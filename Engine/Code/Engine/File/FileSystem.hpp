#pragma once
#include <vector>
#include <string>

class FileSystem
{
public:

	static std::vector<std::string> GetAllFileNamesInPath(const std::string& path);
};