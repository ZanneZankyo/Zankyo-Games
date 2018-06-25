#include "Controller.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Core\Console.hpp"

void Controller::Update()
{
	if (InputSystem::WasKeyJustPressed('P'))
		m_viewMode = VIEW_FREE;
	if (InputSystem::WasKeyJustPressed('O'))
		m_viewMode = VIEW_UNITYCHAN;

	IntVector2 cursorPosition = InputSystem::GetCursorPosition();
	IntVector2 screenCenter = InputSystem::GetScreenCenter();
	InputSystem::SetCursorPosition(screenCenter);
	IntVector2 delta = cursorPosition - screenCenter;

	m_camera.m_rotation.y -= delta.x * m_sensitivity;
	m_camera.m_rotation.x -= delta.y * m_sensitivity;

	if (m_camera.m_rotation.x > 89.99f)
		m_camera.m_rotation.x = 89.99f;
	if (m_camera.m_rotation.x < -89.99f)
		m_camera.m_rotation.x = -89.99f;

	if (m_viewMode == VIEW_FREE)
	{
		m_camera.Update();
		//m_camera.m_position += m_camera.GetForward3D() * 0.01f;
	}
	if (m_viewMode == VIEW_UNITYCHAN)
	{
		Vector3 socketPosition = m_target->GetSocketPosition("Character1_Hips");
		Vector3 cameraForward = m_camera.GetForward3D();
		/*socketPosition.x = 0;
		socketPosition.z = 0;
		socketPosition.y = 100;*/
		m_camera.m_position = /*m_target->m_position +*/ socketPosition - cameraForward * m_cameraDistance;
		//Console::Log("Camera Position: (" + std::to_string(m_camera.m_position.x) + "," + std::to_string(m_camera.m_position.z) + ")");
	}
	
}
