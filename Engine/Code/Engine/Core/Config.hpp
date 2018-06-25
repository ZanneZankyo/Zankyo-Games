#pragma once
#include <string>
#include <utility>
#include <vector>
#include <map>

struct Config
{
private:
	static std::map<std::string, std::string> s_configs;

public:

	// Load a config file
	// see sample.config 
	static bool LoadFile(const std::string& configFilePath);

	// Startup Config System - Setup Initial Configs by parsing supplied file.
	static bool SystemStartup(const std::string& configFilePath);
	static void SystemShutdown();

	// Setters - feel free to use std::string
	static void Set(const std::string& name, const std::string& value);
	static void Set(const std::string& name, int const value);
	static void Set(const std::string& name, float const value);
	static void Set(const std::string& name, bool const value);

	// Easy check for simple on/off flags. 
	static bool IsSet(const std::string& id);
	static void Unset(const std::string& id);

	// Fetching.  If a config exists and is 
	// convertible to the desried type, 
	// place its converted value in the out variable,
	// and return true;
	// Otherwise, return false.
	static bool GetString(std::string& outValue, const std::string& id);
	static bool GetBool(bool& outValue, const std::string& id);
	static bool GetInt(int& outValue, const std::string& id);
	static bool GetFloat(float& outValue, const std::string& id);

	// [DEBUG FEATURE] List all currently existing configs.
	static void List();



};



