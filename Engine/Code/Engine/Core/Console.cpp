#include "Console.hpp"
#include "EngineBase.hpp"
#include "Config.hpp"
#include "StringUtils.hpp"
#include <utility>
#include "Engine\RHI\VertexBuffer.hpp"
#include "Engine\Math\IntVector2.hpp"


Console* Console::s_instance = new Console();
std::vector<std::pair<ConsoleCommandFunc, std::string>> Console::e_onPrintMessage;
std::vector<std::pair<ConsoleCommandFunc, std::string>> Console::e_onRunCommand;
Rgba Console::SUCCESS_COLOR = Rgba::GREEN;
Rgba Console::WARNING_COLOR = Rgba::YELLOW;
Rgba Console::ERROR_COLOR = Rgba::RED;

Console::Console()
	: m_font(nullptr)
	, m_logs()
	, m_isOpen(false)
	, m_commands()
	, m_scale(2.f)
	, m_maxNumOfLogs(10)
	, m_logShowIndexOffset(0)
	, m_lock()
{

	s_instance = this;

	m_logs.push_front(std::pair<Rgba, std::string>(Rgba(0x00ff00ff), "Zankyo Engine"));
	m_logs.push_front(std::pair<Rgba, std::string>(Rgba(0x00ff00ff), "Console Version 1.0"));

	Console::AssignCommand("help", PrintAvailableCommands);
	Console::AssignCommand("clear", ClearLog);
	Console::AssignCommand("print", Print);
}

void Console::RunCommand(const std::string& commandAndArguments)
{
	std::pair<std::string, std::string> commandAttr = StringUtils::SplitAtFirstToken(commandAndArguments, ' ');
	const std::string& command = commandAttr.first;
	const std::string& value = commandAttr.second;
	std::map<std::string, ConsoleCommandFunc>::iterator commandFound = s_instance->m_commands.find(command);
	if (commandFound != s_instance->m_commands.end())
	{
		Log(commandAndArguments, Rgba::WHITE);
		commandFound->second(value);
	}
	else
	{
		std::string errorMessage = "Command \"" + command + "\" not found!";
		Log(errorMessage, Rgba::RED);
	}
	for (auto& eventAndCommandAndArg : e_onRunCommand)
		eventAndCommandAndArg.first(eventAndCommandAndArg.second); // callback(commandAndArgumant);
	
}

void Console::RegisterEvent(
	std::vector<std::pair<ConsoleCommandFunc, std::string>>& consoleEvent,
	ConsoleCommandFunc function,
	const std::string& commandAndArguments
)
{
	consoleEvent.push_back(std::pair<ConsoleCommandFunc, std::string>(function, commandAndArguments));
}

void Console::UnRegisterEvent(std::vector<std::pair<ConsoleCommandFunc, std::string>>& consoleEvent, ConsoleCommandFunc function)
{
	for(int index = 0; index < (int)consoleEvent.size(); index++)
		if (consoleEvent[index].first == function)
		{
			consoleEvent.erase(consoleEvent.begin() + index);
			index--;
		}
}

void Console::ProcessConsoleInput()
{
	m_historyCommands.push_front(m_inputStream);
	m_historyCommandIndex = -1;

	RunCommand(m_inputStream);
	
	m_inputStream.clear();
}

/*
void Console::Setup()
{
	std::string fontName;
	Config::GetString(fontName, "font");
	//s_instance->m_font = Font::CreateFontFromFile(renderer->m_rhiDevice,fontName);

	//clear help
	AssignCommand("help", PrintAvailableCommands);
	AssignCommand("clear", ClearLog);

	float windowHeight = (float)renderer->GetWindowSize().x;
	float lineHeight = s_instance->m_font->m_lineHeight * s_instance->m_scale;
	if(lineHeight!=0)
		s_instance->m_maxNumOfLogs = (int)(windowHeight / lineHeight);
}*/

void Console::SetPosition(const Vector2& position)
{
	s_instance->m_topLeft = position;
}

void Console::SetScale(float scale)
{
	s_instance->m_scale = scale;
}

void Console::Print(const std::string& string)
{
	Log(string);
}

void Console::Log(const std::string& string, const Rgba& color)
{
	SCOPE_LOCK(s_instance->m_lock);
	std::vector<std::string> lines = StringUtils::Split(string, '\n');
	for (auto& line : lines)
		s_instance->m_logs.push_front(std::pair<Rgba, std::string>(color, line));
	for (auto& eventAndCommandAndArg : e_onPrintMessage)
		eventAndCommandAndArg.first(string); // callback(commandAndArgumant);

	//s_instance->m_logs.push_front(std::pair<Rgba, std::string>(color, string));
}

void Console::Clean()
{
	SAFE_DELETE(s_instance);
}

void Console::Open()
{
	s_instance->m_isOpen = true;
}

void Console::Close()
{
	s_instance->m_isOpen = false;
}


bool Console::Toggle()
{
	s_instance->m_isOpen = !s_instance->m_isOpen;
	return s_instance->m_isOpen;
}

bool Console::IsOpen()
{
	return s_instance->m_isOpen;
}

void Console::GetInput()
{
	std::vector<unsigned char> pressedKeys = InputSystem::GetAllCharInput();
	for (const auto& key : pressedKeys)
	{
		if (key == KEYCODE_ENTER || key == '`')
			continue;
		//std::map<int, FontTag>::iterator foundTag = s_instance->m_font->m_tags.find((int)key);
		//if (foundTag != s_instance->m_font->m_tags.end())
		//	s_instance->m_inputStream.push_back(key);
		if (key < s_instance->m_font->m_tags.size())
		{
			auto& tag = s_instance->m_font->m_tags[key];
			if(tag.id!=-1)
				s_instance->m_inputStream.push_back(key);
		}
		if (key == KEYCODE_BACKSPACE && !s_instance->m_inputStream.empty())
			s_instance->m_inputStream.pop_back();
	}
	/*if (InputSystem::WasKeyJustPressed(KEYCODE_BACKSPACE))
	{
		if(!s_instance->m_inputStream.empty())
			s_instance->m_inputStream.pop_back();
	}*/
	if (InputSystem::WasKeyJustPressed(KEYCODE_ENTER))
	{
		if(!s_instance->m_inputStream.empty())
			s_instance->ProcessConsoleInput();
	}
	if (InputSystem::WasKeyJustPressed(KEYCODE_ESCAPE) ||
		InputSystem::WasKeyJustPressed(KEYCODE_GRAVE))
	{
		if (s_instance->m_inputStream.empty())
			s_instance->m_isOpen = false;
		else
			s_instance->m_inputStream.clear();
	}
	if (InputSystem::WasKeyJustPressed(KEYCODE_PAGE_UP))
	{
		s_instance->m_logShowIndexOffset += s_instance->m_maxNumOfLogs - 1;
		if (s_instance->m_logShowIndexOffset > (int)s_instance->m_logs.size() - (int)s_instance->m_maxNumOfLogs)
			s_instance->m_logShowIndexOffset = (int)s_instance->m_logs.size() - (int)s_instance->m_maxNumOfLogs;
	}
	if (InputSystem::WasKeyJustPressed(KEYCODE_PAGE_DOWN))
	{
		s_instance->m_logShowIndexOffset -= s_instance->m_maxNumOfLogs - 1;
		if (s_instance->m_logShowIndexOffset < 0)
			s_instance->m_logShowIndexOffset = 0;
	}
	if (InputSystem::WasKeyJustPressed(KEYCODE_UP))
	{
		s_instance->m_historyCommandIndex++;
		if (s_instance->m_historyCommandIndex < 0)
			s_instance->m_historyCommandIndex = 0;
		if (s_instance->m_historyCommandIndex >= (int)s_instance->m_historyCommands.size())
			s_instance->m_historyCommandIndex = s_instance->m_historyCommands.size() - 1;
		if (!s_instance->m_historyCommands.empty())
			s_instance->m_inputStream = s_instance->m_historyCommands[s_instance->m_historyCommandIndex];
	}
	if (InputSystem::WasKeyJustPressed(KEYCODE_DOWN))
	{
		s_instance->m_historyCommandIndex--;
		if (s_instance->m_historyCommandIndex < 0)
			s_instance->m_historyCommandIndex = 0;
		if (s_instance->m_historyCommandIndex >= (int)s_instance->m_historyCommands.size())
			s_instance->m_historyCommandIndex = s_instance->m_historyCommands.size() - 1;
		if (!s_instance->m_historyCommands.empty())
			s_instance->m_inputStream = s_instance->m_historyCommands[s_instance->m_historyCommandIndex];
	}
}

void Console::AssignCommand(const std::string& command, ConsoleCommandFunc funcPtr)
{
	s_instance->m_commands[command] = funcPtr;
}

void Console::PrintAvailableCommands(const std::string& ignoredParm)
{
	UNUSED(ignoredParm);
	for (const auto& commandAttr : s_instance->m_commands)
	{
		Log(commandAttr.first, Rgba::GREEN);
		//s_instance->m_logs.push_front(std::pair<Rgba, std::string>(Rgba::YELLOW,commandAttr.first));
	}
}

void Console::ClearLog(const std::string& ignoredParm)
{
	UNUSED(ignoredParm);
	s_instance->m_logs.clear();
}

