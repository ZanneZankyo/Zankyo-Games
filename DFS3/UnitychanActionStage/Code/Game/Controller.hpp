#pragma once
#include "Engine\Renderer\CameraZXY.hpp"
#include "Entity.hpp"
#include "Unitychan.hpp"



class Controller
{
public:
	enum eViewMode
	{
		VIEW_FREE,
		VIEW_UNITYCHAN
	};
public:
	eViewMode m_viewMode = VIEW_UNITYCHAN;
	CameraZXY m_camera;
	float m_sensitivity = 0.3f;
	float m_cameraDistance = 250.f;
	Unitychan* m_target = nullptr;
public:
	void Update();
	void Render();
};