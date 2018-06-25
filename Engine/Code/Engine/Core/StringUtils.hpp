#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <utility>
#include <limits>
#include <iosfwd>
#include "Engine\Math\Vector3.hpp"
#include "..\Math\Vector2.hpp"


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

class StringUtils
{
public:
	static std::vector<std::string> Split(const std::string& string, char token);
	static std::pair<std::string, std::string> SplitAtLastToken(const std::string& string, char token);
	static std::pair<std::string, std::string> SplitAtFirstToken(const std::string& string, char token);
	static void RemoveCharsInString(std::string& outString, char token);
	static size_t CountToken(const std::string& string, char token);
	static bool HasSubString(const std::string& string, const std::string& subString);
	template <typename StreamType>
	static void IgnoreStreamLine(std::basic_istream<StreamType>& stream);
	static bool ToBool(const std::string& boolString);
	static int GetLineSize(const std::string& string);
	static Vector2 ToVector2(std::string vector3str);
	static Vector3 ToVector3(std::string vector3str);
	static std::string FromByteSize(size_t byteSize);
	static std::wstring ToWstring(const std::string& string);
	static std::string ToString(const std::wstring& wstring);
};

template <typename StreamType>
void StringUtils::IgnoreStreamLine(std::basic_istream<StreamType>& stream)
{
	stream.ignore((std::streamsize)std::numeric_limits<std::streamsize>::max, '\n');
}

