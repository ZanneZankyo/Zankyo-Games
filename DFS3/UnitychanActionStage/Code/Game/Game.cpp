#include "Game.hpp"
#include "Engine\Core\Config.hpp"
#include "GameConfig.hpp"
#include "Engine\Core\Time.hpp"
#include "Engine\Core\EngineBase.hpp"
#include "Engine\Core\Window.hpp"
#include "Engine\RHI\RHITypes.hpp"
#include "Engine\RHI\Texture2D.hpp"
#include "Engine\RHI\Sampler.hpp"
#include "Engine\RHI\ConstantBuffer.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Core\Console.hpp"
#include "Engine\Math\AABB3.hpp"
#include "Engine\RHI\VertexBuffer.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Math\MathUtils.hpp"
#include <math.h>
#include "Engine\Core\RGBAf.hpp"
#include "Engine\Tools\fbx.hpp"
#include <thread>
#include "Engine\RHI\StructuredBuffer.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Math\Matrix4.hpp"
#include "Engine\RHI\RasterState.hpp"
#include "Engine\Tools\Memory.hpp"
#include "Engine\Core\Profiler.hpp"

Game* Game::s_instance;



Game::Game()
	: m_quitReason(QUITREASON_NONE)
	, m_timeOfLastRunframe(0.f)
{
}

Game::~Game()
{
}

void Game::AssignConsoleCommands()
{
	Console::AssignCommand("full_screen", ToggleFullScreen);
	Console::AssignCommand("full_screen_borderless", ToggleFullScreenBorderless);
}

void Game::Run()
{
	Start();
	while (IsRunning()) {
		RunFrame();
	}
	End();
}

void Game::Start()
{

	InitRendering();

	int screenWidth;
	int screenHeight;

	Config::GetInt(screenWidth, "window_width");
	Config::GetInt(screenHeight, "window_height");

	m_renderer.SetupConsole();
	Matrix4::YAW_AXIS = Vector3(0.0f, 1.0f, 0.0f);
	Matrix4::PITCH_AXIS = Vector3(1.0f, 0.0f, 0.0f);
	Matrix4::ROLL_AXIS = Vector3(0.0f, 0.0f, 1.0f);
	//Console::Setup(&m_renderer);
	Console::SetPosition(Vector2(screenWidth * 0.01f, screenHeight * 0.02f));
	Console::SetScale(2.f);

	AssignConsoleCommands();
	ParticleSystem::Start();
	FBXUtils::Setup();

	if (m_danceStage)
		m_danceStage->Start();
	if (m_actionStage)
		m_actionStage->Start();

	Time::Update();
}



void Game::RunFrame()
{
	// Process Window Messages
	//ClockSystemStep(); 
	//InputSystem::GetInstance()->BeginFrame();
	// update the frame time


	float currentTime = (float)GetCurrentTimeSeconds();
	float deltaSeconds = currentTime - m_timeOfLastRunframe;

	m_timeOfLastRunframe = currentTime;

	//m_renderer.GetWindow()->LockMouse();
	m_renderer.GetWindow()->RunMessagePump();

	if (m_renderer.IsClosed()) {
		Quit();
	}
	else {
		Update(deltaSeconds);
		Render();
	}

	InputSystem::GetInstance()->EndFrame();
}

void Game::End()
{
	CleanupRendering();
}

void Game::Update(float deltaSeconds)
{
	Time::Update();
	if (Console::IsOpen())
		Console::GetInput();
	else
		CheckInput(deltaSeconds);
	if (m_danceStage)
		m_danceStage->Update();
	if (m_actionStage)
		m_actionStage->Update();
}

void Game::Render()
{
	m_renderer.SetRenderTarget(nullptr);
	m_renderer.ClearColor(0x222222ff);
	m_renderer.ClearDepth();

	m_renderer.SetViewport();

	m_renderer.SetAmbientLight(Rgba::WHITE, 4.f);

	m_renderer.DisableAllPointLight();

	const IntVector2& windowSize = m_renderer.GetWindowSize();
	float aspectRatio = (float)windowSize.x / (float)windowSize.y;

	m_renderer.EnableDepthTest();
	m_renderer.EnableDepthWrite();

	/*m_renderer.SetCamera();
	m_renderer.SetEyePosition(m_camera.m_position);
	m_renderer.SetViewMatrix(m_camera.GetViewMatrix());
	m_renderer.SetPerspectiveProjection(m_fov, aspectRatio, 0.1f, 50.0f);*/
	m_renderer.SetAmbientLight(Rgba::WHITE, 0.8f);
	m_renderer.SetTexture(nullptr);
	m_renderer.SetNormalTexture(nullptr);
	m_renderer.SetShader(nullptr);
	//m_renderer.EnablePointLight()

	if(m_danceStage)
		m_danceStage->Render();
	if (m_actionStage)
		m_actionStage->Render();

	if (Console::IsOpen())
		m_renderer.DrawConsole();
		//Console::Render();

	
	m_renderer.DisableBlend();

	m_renderer.Present();
}

void Game::Quit(eQuitReason reason)
{
	m_quitReason = reason;
}

void Game::CheckInput(float deltaSeconds)
{
	if (!Console::IsOpen() && InputSystem::GetInstance()->WasKeyJustPressed(KEYCODE_ENTER))
		Console::Open();
	else if (InputSystem::GetInstance()->WasKeyJustPressed(KEYCODE_GRAVE))
		Console::Toggle();
	else if (InputSystem::GetInstance()->WasKeyJustPressed(KEYCODE_ESCAPE))
		Quit();
	else if (InputSystem::GetInstance()->WasKeyJustPressed('U'))
		m_renderer.GetWindow()->TriggerFullScreen();
}

void Game::ToggleFullScreen(const std::string& /*= ""*/)
{
	s_instance->m_renderer.m_rhiOutput->m_window->TriggerFullScreen();
}

void Game::ToggleFullScreenBorderless(const std::string& /*= ""*/)
{
	s_instance->m_renderer.m_rhiOutput->m_window->BorderlessFullScreen();
}

void Game::S_Quit(const std::string& ignoredParm)
{
	UNUSED(ignoredParm);
	s_instance->Quit();
}

void Game::InitRendering()
{
	int width = g_windowWidth;
	int height = g_windowHeight;

	Config::GetInt(width, "window_width");
	Config::GetInt(height, "window_height");

	m_renderer.Setup(width, height);
	m_renderer.SetViewport(0, 0, width, height);
}

void Game::CleanupRendering()
{
	m_renderer.Destroy();
}
/*

void Game::SetupInput()
{
	/ *InputDeviceMouse *mouse = GetMouse();
	InputDeviceKeyboard *keyboard = GetKeyboard();
	// InputDeviceGamepad *gamepad = GetGamepad();

	// Some other mouse things
	mouse->contain(true);
	mouse->show_system_cursor(false);

	// Setup Quit Key
	VirtualInputValue *quit = m_controller.get_input_value("quit");
	quit->watch(keyboard->get_value(KB_ESCAPE));

	// Directions
	InputAxis *forward = m_controller.get_input_axis("forward");
	forward->watch(keyboard->get_value(KB_W), keyboard->get_value(KB_S));

	InputAxis *right = m_controller.get_input_axis("right");
	right->watch(keyboard->get_value(KB_D), keyboard->get_value(KB_A));

	InputAxis *up = m_controller.get_input_axis("up");
	up->watch(keyboard->get_value(KB_SPACEBAR), keyboard->get_value(KB_SHIFTLEFT));

	// Rotation
	InputAxis *yaw = m_controller.get_input_axis("yaw");
	yaw->watch(&mouse->rel_x);

	InputAxis *pitch = m_controller.get_input_axis("pitch");
	pitch->watch(&mouse->rel_y);* /
}*/
