#pragma once
#include <string>
#include <fstream>
#include <set>
#include "Thread.hpp"
#include <map>
#include "RGBA.hpp"
#include "ThreadSafeQueue.hpp"
#include "Signal.hpp"
#include "ThreadEvent.hpp"

class Logger
{
public:
	std::string m_logFileName;
	std::ofstream m_fileStream;

	std::set<std::string> m_enabledTags;
	std::map<std::string, Rgba> m_tagColors;

	bool m_isRunning;
	thread_handle_t m_thread;
	Signal m_logSignal;
	ThreadEvent<std::ofstream&, const std::string&> e_logEvent;
	ThreadEvent<const std::string&, const Rgba&> e_onLog;

	ThreadSafeQueue<std::string> m_stringQueue;

	Logger();
	~Logger();

	static void Start();
	static void End();

	static void LogTagged(const std::string& tag, const std::string& string);
	static void LogPrint(const std::string& string);
	static void LogWarning(const std::string& string);
	static void LogError(const std::string& string);
	static void LogWithCallStack(const std::string& string);

	static void DisableTag(const std::string& tag);
	static void EnableTag(const std::string& tag);

	static void SetTagColor(const std::string& tag, const Rgba& color);

	static void Thread(void*);

	static uint MessageFlush(std::ofstream& stream);

	static void Flush();

private:

	static void PrintToFile(void*, std::ofstream& stream, const std::string& string);
	static void PrintToConsole(void*, const std::string& string, const Rgba& color);

	static Logger* s_instance;
};