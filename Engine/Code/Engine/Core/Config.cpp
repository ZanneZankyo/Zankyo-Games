#include <fstream>
#include "Config.hpp"
#include "ErrorWarningAssert.hpp"

std::map<std::string, std::string> Config::s_configs;

bool Config::LoadFile(const std::string& configFilePath)
{
	std::ifstream file(configFilePath);
	if (file.bad())
		return false;
	std::string line;
	while (std::getline(file, line))
	{
		

		//remove all whitespace
		for (size_t lineIndex = 0; lineIndex < line.size(); lineIndex++)
		{
			if (line[lineIndex] == ' ')
			{
				line.erase(line.begin() + lineIndex);
				lineIndex--;
			}
		}

		std::string key;
		std::string value;

		for (size_t lineIndex = 0; lineIndex < line.size(); lineIndex++)
		{
			if (line[lineIndex] == '=' && lineIndex < line.size() - 1)
			{
				key = line.substr(0, lineIndex);
				value = line.substr(lineIndex + 1, line.size() - lineIndex - 1);
				break;
			}
		}
		if (key.empty() || value.empty() || (key.size() > 0 && key[0] == '#'))
			continue;
		s_configs[key] = value;
	}
	List();
	return true;
}

bool Config::SystemStartup(const std::string & configFilePath)
{
	return LoadFile(configFilePath);
}

void Config::SystemShutdown()
{
}

void Config::Set(const std::string & name, const std::string & value)
{
	s_configs[name] = value;
}

void Config::Set(const std::string & name, int const value)
{
	s_configs[name] = std::to_string(value);
}

void Config::Set(const std::string & name, float const value)
{
	s_configs[name] = std::to_string(value);
}

void Config::Set(const std::string & name, bool const value)
{
	s_configs[name] = std::to_string(value);
}

bool Config::IsSet(const std::string & id)
{
	return s_configs.find(id) != s_configs.end();
}

void Config::Unset(const std::string & id)
{
	std::map<std::string, std::string>::iterator found = s_configs.find(id);
	if (found != s_configs.end())
		s_configs.erase(found);
}

bool Config::GetString(std::string & outValue, const std::string & id)
{
	std::map<std::string, std::string>::iterator found = s_configs.find(id);
	if (found != s_configs.end())
	{
		outValue = found->second;
		return true;
	}
	return false;
}

bool Config::GetBool(bool & outValue, const std::string & id)
{
	std::map<std::string, std::string>::iterator found = s_configs.find(id);
	if (found != s_configs.end())
	{
		outValue = !(found->second == "0" || found->second == "false");
		return true;
	}
	return false;
}

bool Config::GetInt(int & outValue, const std::string & id)
{
	std::map<std::string, std::string>::iterator found = s_configs.find(id);
	if (found != s_configs.end())
	{
		outValue = stoi(found->second);
		return true;
	}
	return false;
}

bool Config::GetFloat(float & outValue, const std::string & id)
{
	std::map<std::string, std::string>::iterator found = s_configs.find(id);
	if (found != s_configs.end())
	{
		outValue = stof(found->second);
		return true;
	}
	return false;
}

void Config::List()
{
	if(!s_configs.empty())
	for (const auto& config : s_configs)
	{
		DebuggerPrintf("%s : %s\n", config.first.c_str(), config.second.c_str());
	}
}
