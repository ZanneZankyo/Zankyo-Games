#include "Engine/Input/InputSystem.hpp"
#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
#include "Engine/Core/Win32Include.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "XboxInput.hpp"

InputSystem* InputSystem::s_instance = new InputSystem();

InputSystem::InputSystem()
	: m_charInput()
	, m_isFocused(true)
{
	for (int keyIndex = 0; keyIndex < NUM_KEYBOARD_KEYS; keyIndex++)
	{
		m_areKeyPressed[keyIndex].m_isDown = false;
		m_areKeyPressed[keyIndex].m_justChanged = false;
	}
}

InputSystem::~InputSystem()
{
}

void InputSystem::RegisterKeyDown(unsigned char keyIndex)
{
	bool wasDown = s_instance->m_areKeyPressed[keyIndex].m_isDown;
	s_instance->m_areKeyPressed[keyIndex].m_isDown = true;

	if (s_instance->m_areKeyPressed[keyIndex].m_isDown != wasDown)
		s_instance->m_areKeyPressed[keyIndex].m_justChanged = true;
	else
		s_instance->m_areKeyPressed[keyIndex].m_justChanged = false;
}

void InputSystem::RegisterKeyUp(unsigned char keyIndex)
{
	bool wasDown = s_instance->m_areKeyPressed[keyIndex].m_isDown;
	s_instance->m_areKeyPressed[keyIndex].m_isDown = false;

	if (s_instance->m_areKeyPressed[keyIndex].m_isDown != wasDown)
		s_instance->m_areKeyPressed[keyIndex].m_justChanged = true;
	else
		s_instance->m_areKeyPressed[keyIndex].m_justChanged = false;
}

void InputSystem::RegisterKeyChar(unsigned char keyChar)
{
	s_instance->m_charInput.push_back(keyChar);
}

void InputSystem::BeginFrame()
{
	XboxInput::StaticBeginFrame();
}

void InputSystem::EndFrame()
{
	s_instance->m_areKeyPressed[KEYCODE_WHEEL_UP].m_isDown = false;
	s_instance->m_areKeyPressed[KEYCODE_WHEEL_DOWN].m_isDown = false;
	for (int keyIndex = 0; keyIndex < NUM_KEYBOARD_KEYS; keyIndex++)
	{
		s_instance->m_areKeyPressed[keyIndex].m_justChanged = false;
	}
	s_instance->m_charInput.clear();
	XboxInput::StaticEndFrame();
}

bool InputSystem::IsKeyDown(unsigned char keyIndex)
{
	return s_instance->m_areKeyPressed[keyIndex].m_isDown;
}

bool InputSystem::WasKeyJustPressed(unsigned char keyIndex)
{
	return IsKeyDown(keyIndex) && s_instance->m_areKeyPressed[keyIndex].m_justChanged;
}

bool InputSystem::WasKeyJustReleased(unsigned char keyIndex)
{
	return !IsKeyDown(keyIndex) && s_instance->m_areKeyPressed[keyIndex].m_justChanged;
}

std::vector<unsigned char> InputSystem::GetAllKeyJustPressed()
{
	std::vector<unsigned char> keys;
	for (int index = 0; index < NUM_KEYBOARD_KEYS; index++)
	{
		if (WasKeyJustPressed((unsigned char)index))
		keys.push_back((unsigned char)index);
	}
	return keys;
}

std::vector<unsigned char> InputSystem::GetAllCharInput()
{
	return s_instance->m_charInput;
}

void InputSystem::SetCursorPosition(const IntVector2& position)
{
	SetCursorPos(position.x, position.y);
}

IntVector2 InputSystem::GetCursorPosition()
{
	POINT rawCursorPos;
	GetCursorPos(&rawCursorPos);
	return IntVector2(rawCursorPos.x, rawCursorPos.y);
}

Vector2 InputSystem::GetNormalizedCursorPosition(const IntVector2& windowTopLeftPosition, const IntVector2& windowSize)
{
	IntVector2 cursorPosition = GetCursorPosition();
	float x = RangeMap((float)cursorPosition.x, (float)windowTopLeftPosition.x, (float)windowTopLeftPosition.x + (float)windowSize.x, 0.f, 1.f);
	float y = 1.f - RangeMap((float)cursorPosition.y, (float)windowTopLeftPosition.y, (float)windowTopLeftPosition.y + (float)windowSize.y, 0.f, 1.f);
	return Vector2(x, y);
}

void InputSystem::ShowMouseCursor(bool isNowVisible)
{
	if (isNowVisible)
	{
		int currentShowCount = ShowCursor(TRUE);
		while (currentShowCount < 0)
		{
			currentShowCount = ShowCursor(TRUE);
		}
	}
	else
	{
		int currentShowCount = ShowCursor(FALSE);
		while (currentShowCount >= 0)
		{
			currentShowCount = ShowCursor(FALSE);
		}
	}
}

void InputSystem::GetInput(std::string& inputStream)
{
	std::vector<unsigned char> pressedKeys = GetAllCharInput();
	for (const auto& key : pressedKeys)
	{
		if (
			   key == KEYCODE_ESCAPE
			|| key == KEYCODE_ENTER
			|| key == KEYCODE_SPACE
			|| key == KEYCODE_UP
			|| key == KEYCODE_DOWN
			|| key == KEYCODE_LEFT
			|| key == KEYCODE_RIGHT
			|| key == KEYCODE_TAB
			|| key == KEYCODE_SHIFT
			|| key == KEYCODE_CTRL
			|| key == KEYCODE_ALT
			|| key == KEYCODE_LMB
			|| key == KEYCODE_RMB
			|| key == KEYCODE_MMB
			|| key == KEYCODE_WHEEL_UP
			|| key == KEYCODE_WHEEL_DOWN
			|| key == KEYCODE_GRAVE
			|| key == KEYCODE_F1
			|| key == KEYCODE_F2
			|| key == KEYCODE_F3
			|| key == KEYCODE_F4
			|| key == KEYCODE_F5
			|| key == KEYCODE_F6
			|| key == KEYCODE_F7
			|| key == KEYCODE_F8
			|| key == KEYCODE_F9
			|| key == KEYCODE_F10
			|| key == KEYCODE_F11
			|| key == KEYCODE_F12
			|| key == KEYCODE_PAGE_UP
			|| key == KEYCODE_PAGE_DOWN
			|| key == KEYCODE_END
			|| key == KEYCODE_HOME
			)
			continue;
		if (key == KEYCODE_BACKSPACE)
		{
			if(!inputStream.empty())
				inputStream.pop_back();
			continue;
		}
		inputStream.push_back(key);
			
	}
}

IntVector2 InputSystem::GetScreenCenter()
{
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	IntVector2 screenCenter;
	screenCenter.x = (desktopRect.right - desktopRect.left) / 2;
	screenCenter.y = (desktopRect.bottom - desktopRect.top) / 2;
	return screenCenter;
}

InputSystem * InputSystem::GetInstance()
{
	return s_instance;
}

void InputSystem::OnGainFocus()
{
	s_instance->m_isFocused = true;
}

void InputSystem::OnLostFocus()
{
	s_instance->m_isFocused = false;
}

bool InputSystem::IsFocused()
{
	return s_instance->m_isFocused;
}
