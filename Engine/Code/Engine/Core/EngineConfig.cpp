#include <fstream>
#include "EngineConfig.hpp"
#include <string>
#include "Config.hpp"

int g_windowWidth = 1280;
int g_windowHeight = 720;
int g_windowResX = 1280;
int g_windowResY = 720;
std::string g_windowTitle = "Zankyo Engine";

void ReadEngineConfig()
{
	Config::GetInt(g_windowResX,"window_res_x");
	Config::GetInt(g_windowResY, "window_res_y");

}
