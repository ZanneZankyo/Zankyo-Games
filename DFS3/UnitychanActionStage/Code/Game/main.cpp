#define WIN32_LEAN_AND_MEAN

//#include "log.h"
#include "Engine\Core\EngineBase.hpp"
#include "Engine\Core\Window.hpp"
#include "Engine\Core\Config.hpp"
#include "Game.hpp"

int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{
	UNUSED(applicationInstanceHandle);
	UNUSED(commandLineString);

	// INITIALIZE ENGINE SYSTEMS NEEDED
	Config::LoadFile("GameConfig.ini");
	Config::LoadFile("EngineConfig.ini");
	//RHISystemStartup();
	//InputSystemStartup(); // Assume you have input systems from previous class

	// Run the game
	// Again, assuming you have an existing
	// class/structure from previous courses

	Game* game = new Game();
	Game::s_instance = game;
	game->Run();

	// Shutdown
	//InputSystemShutdown();
	//RHISystemShutdown();
	Config::SystemShutdown();

	return 0;
}