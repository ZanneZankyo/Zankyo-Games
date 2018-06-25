#pragma once
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Matrix4.hpp"
#include "Engine\Math\IntVector2.hpp"

// Camera control that set forward as Z-axis, right as X-axis, up as Y-axis
class CameraZXY
{

public:
	CameraZXY();
	CameraZXY(const CameraZXY& copy);
	explicit CameraZXY(const Vector3& position, const Vector3& rotation = Vector3::ZERO, bool isControllableByKeyboard = false, bool isControllableByMouse = false);

	virtual Matrix4 GetViewMatrix() const;
	virtual void Update(float deltaSeconds);
	virtual void Update();

	IntVector2 GetMouseDelta() const;

	Vector3 GetForward3D() const;
	Vector3 GetForwardXY() const;
	Vector3 GetRight3D() const;
	Vector3 GetLeftXYZ() const;
	Vector3 GetRightXY() const;

public:
	Vector3 m_position;
	Vector3 m_rotation;
	bool m_isControllableByKeyboard;
	bool m_isControllableByMouse;
	float m_near;
	float m_far;
	float m_fov;
public:
	static float s_moveSpeed;
	static float s_turnSpeedDegrees;
	static float s_mouseSensitivityX;
	static float s_mouseSensitivityY;
	static bool s_doInvertMouseY;
};

inline CameraZXY::CameraZXY()
	: m_position()
	, m_rotation()
	, m_isControllableByKeyboard(true)
	, m_isControllableByMouse(false)
	, m_fov(60.f)
	, m_near(0.1f)
	, m_far(50.f)
{}

inline CameraZXY::CameraZXY(const CameraZXY& copy)
	: m_position(copy.m_position)
	, m_rotation(copy.m_rotation)
	, m_isControllableByKeyboard(copy.m_isControllableByKeyboard)
	, m_isControllableByMouse(copy.m_isControllableByMouse)
	, m_fov(copy.m_fov)
	, m_near(copy.m_near)
	, m_far(copy.m_far)
{}

inline CameraZXY::CameraZXY(const Vector3& position, const Vector3& rotation/* = Vector3::ZERO*/, bool isControllableByKeyboard/* = false*/, bool isControllableByMouse/* = false*/)
	: m_position(position)
	, m_rotation(rotation)
	, m_isControllableByKeyboard(isControllableByKeyboard)
	, m_isControllableByMouse(isControllableByMouse)
	, m_fov(60.f)
	, m_near(0.1f)
	, m_far(50.f)
{}