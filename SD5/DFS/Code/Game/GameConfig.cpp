#include <fstream>
#include "GameConfig.hpp"
#include <string>
#include "Engine\Core\Config.hpp"

int g_windowWidth = 3840;
int g_windowHeight = 2048;
std::string g_windowTitle = "DFS";

void ReadGameConfig()
{
	Config::LoadFile("GameConfig.ini");
	Config::GetInt(g_windowWidth, "window_width");
	Config::GetInt(g_windowHeight, "window_height");
	Config::GetString(g_windowTitle, "window_title");
}
