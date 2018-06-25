#include "CameraXYZ.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Math\MathUtils.hpp"

float CameraXYZ::s_turnSpeedDegrees = 90.f;
float CameraXYZ::s_moveSpeed = 1.f;
float CameraXYZ::s_mouseSensitivityX = 0.25f;
float CameraXYZ::s_mouseSensitivityY = 0.25f;
bool  CameraXYZ::s_doInvertMouseY = false;

void CameraXYZ::Update(float deltaSeconds)
{

	if (InputSystem::IsKeyDown(KEYCODE_UP))
		m_rotation.y -= deltaSeconds * s_turnSpeedDegrees;
	if (InputSystem::IsKeyDown(KEYCODE_DOWN))
		m_rotation.y += deltaSeconds * s_turnSpeedDegrees;
	if (InputSystem::IsKeyDown(KEYCODE_LEFT))
		m_rotation.z += deltaSeconds * s_turnSpeedDegrees;
	if (InputSystem::IsKeyDown(KEYCODE_RIGHT))
		m_rotation.z -= deltaSeconds * s_turnSpeedDegrees;

	if (m_rotation.y < -89.99f)
		m_rotation.y = -89.99f;
	if (m_rotation.y > 89.99f)
		m_rotation.y = 89.99f;

	if (m_isControllableByMouse)
	{
		IntVector2 mouseDelta = GetMouseDelta();

		m_rotation.y += static_cast<float>(mouseDelta.y) * s_mouseSensitivityY * (s_doInvertMouseY ? -1.f : 1.f);
		m_rotation.z -= static_cast<float>(mouseDelta.x) * s_mouseSensitivityX;
	}
}

IntVector2 CameraXYZ::GetMouseDelta() const
{
	IntVector2 mouseScreenPos = InputSystem::GetCursorPosition();
	IntVector2 screenCenter = InputSystem::GetScreenCenter();
	return mouseScreenPos - screenCenter;
}

Vector3 CameraXYZ::GetForwardXYZ() const
{
	return Vector3(CosDegrees(m_rotation.z) * CosDegrees(m_rotation.y)
		, SinDegrees(m_rotation.z) * CosDegrees(m_rotation.y)
		, -SinDegrees(m_rotation.y));
}

Vector3 CameraXYZ::GetForwardXY() const
{
	return Vector3(CosDegrees(m_rotation.z), SinDegrees(m_rotation.z), 0.f);
}

Vector3 CameraXYZ::GetLeftXY() const
{
	Vector3 forward = GetForwardXY();
	return Vector3(-forward.y, forward.x, 0.f);
}
