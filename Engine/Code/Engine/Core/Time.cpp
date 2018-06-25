//-----------------------------------------------------------------------------------------------
// Time.cpp
//	A simple high-precision time utility function for Windows
//	based on code by Squirrel Eiserloh
//-----------------------------------------------------------------------------------------------

#include "Engine/Core/Time.hpp"
#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
#include "Win32Include.hpp"
#include <ctime>
#include <sstream>
#include <iomanip>
#include "EngineBase.hpp"
#include <time.h>

//-----------------------------------------------------------------------------------------------
double InitializeTime( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );
	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}


//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}


float Time::s_currentTimeSeconds = 0.f;
float Time::s_lastTimeSeconds = 0.f;
float Time::deltaSeconds = 0.f;

void Time::Update()
{
	s_currentTimeSeconds = (float)GetCurrentTimeSeconds();
	deltaSeconds = s_currentTimeSeconds - s_lastTimeSeconds;

	s_lastTimeSeconds = s_currentTimeSeconds;
}

void Time::Setup()
{
	deltaSeconds = 0.f;
	s_lastTimeSeconds = (float)GetCurrentTimeSeconds();
}

std::string Time::ToString(float timeSeconds)
{
	std::stringstream ss;
	if(timeSeconds > 1.f)
		ss << std::setprecision(2) << std::fixed << timeSeconds << " s";
	else if(timeSeconds > 1 MS)
		ss << std::setprecision(2) << std::fixed << timeSeconds * 1000.f << " ms";
	else
		ss << std::setprecision(2) << std::fixed << timeSeconds * 1000000.f << " us";
	return ss.str();
}

std::string Time::GetSystemTimeString(const std::string& format)
{
	std::time_t time = std::time(0);
	tm* timeStruct = new tm();
	localtime_s(timeStruct, &time);
	char timeStr[128];
	if(!std::strftime(timeStr, 128, format.c_str(), timeStruct))
		std::strftime(timeStr, 128, "%T", timeStruct);
	return timeStr;
	/*std::stringstream ss;
	ss << timeStruct->tm_mon + 1 << '_' << timeStruct->tm_mday << "__"
		<< timeStruct->tm_hour << '_' << timeStruct->tm_min << '_' << timeStruct->tm_sec;
	//std::string timeString = std::asctime(timeStruct);
	return ss.str();*/
}
