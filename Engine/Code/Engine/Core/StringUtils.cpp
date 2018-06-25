#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>
#include "ErrorWarningAssert.hpp"
#include <sstream>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}


std::vector<std::string> StringUtils::Split(const std::string& string, char token)
{
	int offset = 0;
	std::vector<std::string> subStrings;
	while (offset < (int)string.size())
	{
		if(string[offset] == token)
			while (offset < (int)string.size() && string[offset] == token)offset++; //skip multiple token
		int current = offset;
		std::string subString = "";
		while (current < (int)string.size() && string[current] != token)current++; //find next token
		subString = string.substr(offset, current - offset);
		subStrings.push_back(subString);
		offset = current + 1;
	}
	return subStrings;
}

std::pair<std::string, std::string> StringUtils::SplitAtLastToken(const std::string& string, char token)
{
	int endSplitPoint = string.size() - 1;
	while (endSplitPoint >= 0 && string[endSplitPoint] != token)endSplitPoint--; //find next token

	if (endSplitPoint < 0 || endSplitPoint >= (int)string.size() - 1)
		return std::pair<std::string, std::string>(string, "");

	int beginSplitPoint = endSplitPoint;
	while (beginSplitPoint >= 0 && string[beginSplitPoint] == token)beginSplitPoint--; //find next token

	std::string first = string.substr(0, beginSplitPoint + 1);
	std::string second = string.substr(endSplitPoint + 1, string.size() - (endSplitPoint + 1));

	return std::pair<std::string, std::string>(first,second);
}

std::pair<std::string, std::string> StringUtils::SplitAtFirstToken(const std::string& string, char token)
{
	int beginSplitPoint = 0;
	while (beginSplitPoint < (int)string.size() && string[beginSplitPoint] != token)beginSplitPoint++; //find next token

	if (beginSplitPoint < 0 || beginSplitPoint >= (int)string.size() - 1)
		return std::pair<std::string, std::string>(string, "");

	int endSplitPoint = beginSplitPoint;
	while (endSplitPoint < (int)string.size() && string[endSplitPoint] == token)endSplitPoint++; //find next token

	std::string first = string.substr(0, beginSplitPoint);
	std::string second = string.substr(endSplitPoint, string.size() - endSplitPoint);

	return std::pair<std::string, std::string>(first, second);
}

void StringUtils::RemoveCharsInString(std::string& outString, char token)
{
	for (size_t index = 0; index < outString.size(); index++)
	{
		if (outString[index] == token)
		{
			outString.erase(outString.begin() + index);
			index--;
		}
	}
}

size_t StringUtils::CountToken(const std::string& string, char token)
{
	size_t count = 0;
	for (auto& c : string)
		if (c == token)
			count++;
	return count;
}

bool StringUtils::HasSubString(const std::string& string, const std::string& subString)
{
	if (string.empty()||subString.empty())
		return false;
	char startChar = subString[0];
	for (size_t charIndex = 0; charIndex < string.size(); charIndex++)
	{
		if (string[charIndex] == startChar)
		{
			size_t subIndex = 0;
			size_t currentIndex = charIndex;
			while 
				(
					currentIndex < string.size() && 
					subIndex < subString.size() && 
					string[currentIndex] == subString[subIndex]
				)
			{
				subIndex++;
				currentIndex = charIndex + subIndex;
			}
			if (subIndex == subString.size())
				return true;
		}
	}
	return false;
}

bool StringUtils::ToBool(const std::string& boolString)
{
	return boolString == "true" || boolString == "True" || boolString == "TRUE";
}

int StringUtils::GetLineSize(const std::string& string)
{
	int lineSize = 1;
	for (auto c : string)
		if (c == '\n')
			lineSize++;
	return lineSize;
}

Vector2 StringUtils::ToVector2(std::string vector2str)
{
	auto vecStrs = Split(vector2str, ',');
	ASSERT_OR_DIE(vecStrs.size() == 2, "StringUtils::ToVector2(): \"" + vector2str + "\" Failed to convert into Vector2!\n");
	float x = stof(vecStrs[0]);
	float y = stof(vecStrs[1]);
	return Vector2(x, y);
}

Vector3 StringUtils::ToVector3(std::string vector3str)
{
	auto vecStrs = Split(vector3str, ',');
	ASSERT_OR_DIE(vecStrs.size() == 3, "StringUtils::ToVector3(): \"" + vector3str + "\" Failed to convert into Vector3!\n");
	float x = stof(vecStrs[0]);
	float y = stof(vecStrs[1]);
	float z = stof(vecStrs[2]);
	return Vector3(x, y, z);
}

std::string StringUtils::FromByteSize(size_t byteSize)
{
	std::stringstream ss;

	if (byteSize >= 1 GB)
		ss << std::setprecision(2) << std::fixed << byteSize / (float)(1 GB) << " GB";
	else if (byteSize >= 1 MB)
		ss << std::setprecision(2) << std::fixed << byteSize / (float)(1 MB) << " MB";
	else if (byteSize >= 1 KB)
		ss << std::setprecision(2) << std::fixed << byteSize / (float)(1 KB) << " KB";
	else
		ss << std::setprecision(2) << std::fixed << byteSize << " B";
	return ss.str();
}

std::wstring StringUtils::ToWstring(const std::string& string)
{
	std::wstring wstr(string.length(), L' '); // Make room for characters
	std::copy(string.begin(), string.end(), wstr.begin());
	return wstr;
}

std::string StringUtils::ToString(const std::wstring& wstring)
{
	std::string str(wstring.begin(), wstring.end());
	return str;
}
