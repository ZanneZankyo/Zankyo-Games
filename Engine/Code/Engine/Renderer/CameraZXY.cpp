#include "CameraZXY.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\Time.hpp"

float CameraZXY::s_turnSpeedDegrees = 90.f;
float CameraZXY::s_moveSpeed = 1.f;
float CameraZXY::s_mouseSensitivityX = 0.25f;
float CameraZXY::s_mouseSensitivityY = 0.25f;
bool  CameraZXY::s_doInvertMouseY = false;

Matrix4 CameraZXY::GetViewMatrix() const
{
	Matrix4 mat = Matrix4::CreateIdentity();
	mat.RotateDegreesAboutX(m_rotation.x);
	mat.RotateDegreesAboutY(m_rotation.y);
	mat.Translate(-m_position);
	return mat;
}

void CameraZXY::Update()
{
	Update(Time::deltaSeconds);
}

void CameraZXY::Update(float deltaSeconds)
{
	if (m_isControllableByKeyboard)
	{
		if (InputSystem::IsKeyDown(KEYCODE_UP))
			m_rotation.x -= deltaSeconds * s_turnSpeedDegrees;
		if (InputSystem::IsKeyDown(KEYCODE_DOWN))
			m_rotation.x += deltaSeconds * s_turnSpeedDegrees;
		if (InputSystem::IsKeyDown(KEYCODE_LEFT))
			m_rotation.y += deltaSeconds * s_turnSpeedDegrees;
		if (InputSystem::IsKeyDown(KEYCODE_RIGHT))
			m_rotation.y -= deltaSeconds * s_turnSpeedDegrees;

		float moveSpeed = s_moveSpeed;
		if (InputSystem::IsKeyDown(KEYCODE_SHIFT))
			moveSpeed *= 10.f;

		if (InputSystem::IsKeyDown('W'))
			m_position += GetForwardXY() * moveSpeed * deltaSeconds;
		if (InputSystem::IsKeyDown('S'))
			m_position -= GetForwardXY() * moveSpeed * deltaSeconds;
		if (InputSystem::IsKeyDown('A'))
			m_position -= GetRightXY() * moveSpeed * deltaSeconds;
		if (InputSystem::IsKeyDown('D'))
			m_position += GetRightXY() * moveSpeed * deltaSeconds;
		if (InputSystem::IsKeyDown('R'))
			m_position.y += moveSpeed * deltaSeconds;
		if (InputSystem::IsKeyDown('F'))
			m_position.y -= moveSpeed * deltaSeconds;

		if (m_rotation.x < -89.99f)
			m_rotation.x = -89.99f;
		if (m_rotation.x > 89.99f)
			m_rotation.x = 89.99f;
	}


	if (m_isControllableByMouse)
	{
		IntVector2 mouseDelta = GetMouseDelta();

		m_rotation.x -= static_cast<float>(mouseDelta.y) * s_mouseSensitivityY * (s_doInvertMouseY ? -1.f : 1.f);
		m_rotation.z -= static_cast<float>(mouseDelta.x) * s_mouseSensitivityX;
	}
}

IntVector2 CameraZXY::GetMouseDelta() const
{
	IntVector2 mouseScreenPos = InputSystem::GetCursorPosition();
	IntVector2 screenCenter = InputSystem::GetScreenCenter();
	return mouseScreenPos - screenCenter;
}

Vector3 CameraZXY::GetForward3D() const
{
	Matrix4 viewMat = GetViewMatrix().GetInverse();
	Vector4 i, j, k, t;
	viewMat.GetBasis(i, j, k, t);
	return Vector3(k);
	/*return Vector3(SinDegrees(-m_rotation.x)
		, CosDegrees(m_rotation.y) * CosDegrees(-m_rotation.x)
		, SinDegrees(m_rotation.y) * CosDegrees(-m_rotation.x));*/
}

Vector3 CameraZXY::GetForwardXY() const
{
	Vector3 left = GetRightXY();
	return Vector3(-left.z, 0.f, left.x);
	//return Vector3(SinDegrees(m_rotation.y), 0.f, CosDegrees(m_rotation.y));
}

Vector3 CameraZXY::GetRight3D() const
{
	Matrix4 viewMat = GetViewMatrix().GetInverse();
	Vector4 i, j, k, t;
	viewMat.GetBasis(i, j, k, t);
	return Vector3(i);
}

Vector3 CameraZXY::GetRightXY() const
{
	//Vector3 forward = GetForwardXY();
	return Vector3(CosDegrees(m_rotation.y), 0.f, SinDegrees(m_rotation.y));
}
