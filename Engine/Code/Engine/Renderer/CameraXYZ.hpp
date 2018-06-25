#pragma once
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Matrix4.hpp"
#include "Engine\Math\IntVector2.hpp"

//Camera control that set forward as X-axis, left as Y-axis, up as Z-axis
class CameraXYZ
{

public:
	CameraXYZ();
	CameraXYZ(const CameraXYZ& copy);
	explicit CameraXYZ(const Vector3& position, const Vector3& rotation = Vector3::ZERO, bool isControllable = false);

	virtual Matrix4 GetViewMatrix() const;
	virtual void Update(float deltaSeconds);

	IntVector2 GetMouseDelta() const;

	Vector3 GetForwardXYZ() const;
	Vector3 GetForwardXY() const;
	Vector3 GetLeftXYZ() const;
	Vector3 GetLeftXY() const;

public:
	Vector3 m_position;
	Vector3 m_rotation;
	bool m_isControllableByMouse;

public:
	static float s_moveSpeed;
	static float s_turnSpeedDegrees;
	static float s_mouseSensitivityX;
	static float s_mouseSensitivityY;
	static bool s_doInvertMouseY;
};

inline CameraXYZ::CameraXYZ()
	:m_position()
	,m_rotation()
	,m_isControllableByMouse(false)
{}

inline CameraXYZ::CameraXYZ(const CameraXYZ& copy)
	:m_position(copy.m_position)
	,m_rotation(copy.m_rotation)
	, m_isControllableByMouse(copy.m_isControllableByMouse)
{}

inline CameraXYZ::CameraXYZ(const Vector3& position, const Vector3& rotation/* = Vector3::ZERO*/, bool isControllable/* = false*/)
	:m_position(position)
	,m_rotation(rotation)
	,m_isControllableByMouse(isControllable)
{}