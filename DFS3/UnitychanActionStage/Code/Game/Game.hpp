#pragma once

#include "Engine/RHI/RHI.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/D3D11Renderer.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Renderer/CameraZXY.hpp"
#include "Engine/Tools/fbx.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Renderer/Motion.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"
#include "Engine/Core/EventTimeline.hpp"
#include "SpringJoint.hpp"
#include "UnitychanDancingStage.hpp"
#include "UnitychanActionStage.hpp"

enum eQuitReason
{
	QUITREASON_NONE,
	QUITREASON_USER,
};

class Game
{
public:
	Game();
	~Game();
	void Run();
	void Start();
	void AssignConsoleCommands();
	void RunFrame();
	void End();

	void InitRendering();
	void CleanupRendering();

	void Update(float deltaSeconds);

	void Render();

	void Quit(eQuitReason reason = QUITREASON_USER);
	bool IsQuitting() const;
	bool IsRunning() const;
	void CheckInput(float deltaSeconds);

	static void ToggleFullScreen(const std::string& = "");
	static void ToggleFullScreenBorderless(const std::string& = "");
	static void S_Quit(const std::string& = "");
	static Game* GetInstance() { return s_instance; }

//Universal members
public:
	D3D11Renderer m_renderer;
	AudioSystem m_audio;

	eQuitReason m_quitReason = QUITREASON_NONE;
	float m_timeOfLastRunframe = 0.f;

	static Game *s_instance;

//Game specific members
public:
	UnitychanDancingStage* m_danceStage = nullptr;//new UnitychanDancingStage();//
	UnitychanActionStage* m_actionStage = new UnitychanActionStage();
};


inline bool Game::IsQuitting() const { return m_quitReason != QUITREASON_NONE; }
inline bool Game::IsRunning() const { return !IsQuitting(); }