#include "Logger.hpp"
#include "Time.hpp"
#include "Console.hpp"
#include "ErrorWarningAssert.hpp"
#include "Engine\Tools\CallStack.hpp"

void Logger::Thread(void*)
{
	if (!s_instance)
		return;

	std::ofstream& stream = s_instance->m_fileStream;
	stream.open(s_instance->m_logFileName, std::ios_base::out);

	if (stream.bad())
		return;

	stream << "Game Started" << std::endl;

	s_instance->e_logEvent.subscribe(&stream, PrintToFile);
	s_instance->e_onLog.subscribe(nullptr, PrintToConsole);

	std::string msg;

	while (s_instance->m_isRunning) {
		/*if (s_instance->m_stringQueue.pop(&msg)) {
			stream << msg << std::endl;
		}
		ThreadSleep(1);*/
		s_instance->m_logSignal.wait();
		MessageFlush(stream);
	}

	//stream.close();
}

uint Logger::MessageFlush(std::ofstream& stream)
{
	uint count = 0;
	std::string msg;

	while (s_instance->m_stringQueue.pop(&msg)) {
		s_instance->e_logEvent.trigger(stream,msg);
		++count;
	}

	return count;
}

void Logger::Flush()
{
	ThreadFlush(s_instance->m_thread);
}

Logger* Logger::s_instance = nullptr;

Logger::Logger()
	: m_fileStream()
	, m_logFileName()
{
	m_tagColors["warning"] = Rgba::YELLOW;
	m_tagColors["error"] = Rgba::RED;
}

Logger::~Logger()
{
	m_fileStream.close();
	
}

void Logger::Start()
{
	s_instance = new Logger();
	s_instance->m_logFileName = "data/log/log_"+Time::GetSystemTimeString("%m_%d__%H_%M_%S")+".txt";
	s_instance->m_thread = ThreadCreate(Thread, nullptr);
	s_instance->m_isRunning = true;
	Console::AssignCommand("log_print", Logger::LogPrint);
	Console::AssignCommand("log_enable_tag", Logger::EnableTag);
	Console::AssignCommand("log_disable_tag", Logger::DisableTag);
}

void Logger::End()
{
	if (!s_instance)
		return;
	s_instance->m_isRunning = false;
	s_instance->m_logSignal.signal_all();
	if(s_instance->m_thread)
		ThreadJoin(s_instance->m_thread);
	SAFE_DELETE(s_instance);
}

void Logger::LogTagged(const std::string& tag, const std::string& string)
{
	if (!s_instance || !s_instance->m_enabledTags.empty() && s_instance->m_enabledTags.find(tag) == s_instance->m_enabledTags.end())
		return;
	std::string stringPrint = "[" + Time::GetSystemTimeString("%H:%M:%S") + "][" + tag +"]"+ string;
	s_instance->m_stringQueue.push(stringPrint);
	s_instance->m_logSignal.signal_all();
	//PrintToFile(nullptr,stringPrint);
	Rgba consoleStringColor = Rgba::WHITE;
	auto found = s_instance->m_tagColors.find(tag);
	if (found != s_instance->m_tagColors.end())
		consoleStringColor = found->second;
	//PrintToConsole(stringPrint, consoleStringColor);
	s_instance->e_onLog.trigger(stringPrint, consoleStringColor);
}

void Logger::LogPrint(const std::string& string)
{
	LogTagged("Default" , string);
}

void Logger::LogWarning(const std::string& string)
{
	LogTagged("Warning", string);
}

void Logger::LogError(const std::string& string)
{
	LogTagged("Error", string);
	ASSERT_RECOVERABLE(false, "Error Triggered");
}

void Logger::LogWithCallStack(const std::string& string)
{
	CallStack* cs = CreateCallStack(1);
	std::string callStackString = CallStackGetString(cs, 128);
	LogTagged("callstack", string + "\n" + callStackString);
}

void Logger::DisableTag(const std::string& tag)
{
	s_instance->m_enabledTags.erase(tag);
}

void Logger::EnableTag(const std::string& tag)
{
	s_instance->m_enabledTags.insert(tag);
}

void Logger::SetTagColor(const std::string& tag, const Rgba& color)
{
	s_instance->m_tagColors[tag] = color;
}

void Logger::PrintToFile(void*, std::ofstream& stream, const std::string& string)
{
	//s_instance->m_stringQueue.push(string);
	stream << string;
	
}

void Logger::PrintToConsole(void*, const std::string& string, const Rgba& color)
{
	Console::Log(string, color);
}
