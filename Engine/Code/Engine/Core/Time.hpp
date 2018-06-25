//-----------------------------------------------------------------------------------------------
// Time.hpp
//	A simple high-precision time utility function for Windows
//	based on code by Squirrel Eiserloh
//-----------------------------------------------------------------------------------------------

#pragma once
#include <string>

double GetCurrentTimeSeconds();
double GetCurrentTimeMillisecond();

class Time
{
private:
	
	static float s_lastTimeSeconds;

public:

	static float deltaSeconds;
	static float s_currentTimeSeconds;

	static void Update();
	static void Setup();

	static std::string ToString(float timeSeconds);

	static std::string GetSystemTimeString(const std::string& format = "%T");
};

