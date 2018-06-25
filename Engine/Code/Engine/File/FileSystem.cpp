#include "FileSystem.hpp"
#include "Engine/Core/Window.hpp"

std::vector<std::string> FileSystem::GetAllFileNamesInPath(const std::string & path)
{
	std::vector<std::string> fileNames;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	//_tprintf(TEXT("Target file is %s\n"), argv[1]);
	std::wstring str(path.begin(), path.end());
	hFind = FindFirstFile(str.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return fileNames;
	}
	do
	{
		std::wstring file(FindFileData.cFileName);
		std::string str(file.begin(), file.end());
		fileNames.push_back(str);
	} while (FindNextFile(hFind, &FindFileData) != 0);

	FindClose(hFind);
	return fileNames;
}
