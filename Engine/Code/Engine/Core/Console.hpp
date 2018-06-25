#pragma once
#include "Engine\RHI\Font.hpp"
#include "Engine\Input\InputSystem.hpp"
#include <sstream>
#include <string>
#include <vector>
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\IntVector2.hpp"
#include <utility>
#include <deque>
#include "RGBA.hpp"
#include "criticalsection.hpp"

typedef void(*ConsoleCommandFunc)(const std::string&);

class Console
{
public:
	Font* m_font;
	std::deque<std::pair<Rgba, std::string>> m_logs;
	std::string m_inputStream;
	int m_inputCursorPosition;
	Vector2 m_topLeft;
	IntVector2 m_size;
	std::map<std::string, ConsoleCommandFunc> m_commands;
	float m_scale;
	unsigned int m_maxNumOfLogs;
	int m_logShowIndexOffset;
	std::deque<std::string> m_historyCommands;
	int m_historyCommandIndex = -1;
public:
	//static void Setup();
	static void SetPosition(const Vector2& position);
	static void SetScale(float scale);
	static void Print(const std::string& string);
	static void Log(const std::string& string, const Rgba& color = Rgba::WHITE);
	static void Clean();
	static void Open();
	static void Close();
	static bool Toggle();
	static bool IsOpen();
	static void GetInput();
	static void AssignCommand(const std::string& command, ConsoleCommandFunc funcPtr);
	static void PrintAvailableCommands(const std::string& ignoredParm = "");
	static void ClearLog(const std::string& ignoredParm = "");
	static void RunCommand(const std::string& commandAndArguments);
public:
	static void RegisterEvent(
		std::vector<std::pair<ConsoleCommandFunc, std::string>>& consoleEvent, 
		ConsoleCommandFunc function, 
		const std::string& commandAndArguments = ""
	);
	static void UnRegisterEvent(std::vector<std::pair<ConsoleCommandFunc, std::string>>& consoleEvent, ConsoleCommandFunc function);
	
	static std::vector<std::pair<ConsoleCommandFunc, std::string>> e_onPrintMessage;
	static std::vector<std::pair<ConsoleCommandFunc, std::string>> e_onRunCommand;
private:
	Console();
	bool IsValid() const;
	void ProcessConsoleInput();

	CriticalSection m_lock;
	bool m_isOpen;
public:
	static Console* s_instance;

	static Rgba SUCCESS_COLOR;
	static Rgba WARNING_COLOR;
	static Rgba ERROR_COLOR;
};

inline bool Console::IsValid() const
{
	return m_font != nullptr/* && m_renderer != nullptr*/;
}