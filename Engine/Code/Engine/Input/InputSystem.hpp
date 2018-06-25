#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\IntVector2.hpp"
#include <vector>

const int NUM_KEYBOARD_KEYS = 256;

enum KeyCode
{
	KEYCODE_ESCAPE		= 0x1B,
	KEYCODE_ENTER		= 0x0D,
	KEYCODE_SPACE		= 0x20,
	KEYCODE_UP			= 0x26,
	KEYCODE_DOWN		= 0x28,
	KEYCODE_LEFT		= 0x25,
	KEYCODE_RIGHT		= 0x27,
	KEYCODE_BACKSPACE	= 0x08,
	KEYCODE_TAB			= 0x09,
	KEYCODE_SHIFT		= 0x10,
	KEYCODE_CTRL		= 0x11,
	KEYCODE_ALT			= 0x12,
	KEYCODE_LMB			= 0x01,
	KEYCODE_RMB			= 0x02,
	KEYCODE_MMB			= 0x03,
	KEYCODE_WHEEL_UP	= 0x04,
	KEYCODE_WHEEL_DOWN	= 0x05,
	KEYCODE_GRAVE		= 192,
	KEYCODE_F1			= 0x70,
	KEYCODE_F2			= 0x71,
	KEYCODE_F3			= 0x72,
	KEYCODE_F4			= 0x73,
	KEYCODE_F5			= 0x74,
	KEYCODE_F6			= 0x75,
	KEYCODE_F7			= 0x76,
	KEYCODE_F8			= 0x77,
	KEYCODE_F9			= 0x78,
	KEYCODE_F10			= 0x79,
	KEYCODE_F11			= 0x7A,
	KEYCODE_F12			= 0x7B,
	KEYCODE_PAGE_UP		= 0x21,
	KEYCODE_PAGE_DOWN	= 0x22,
	KEYCODE_END			= 0x23,
	KEYCODE_HOME		= 0x24,

	NUM_KEYCODES = 256,
};

struct KeyState
{
	bool m_isDown;
	bool m_justChanged;
	bool m_isSystemPressed;
};

class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	std::vector<unsigned char> m_charInput;
	bool m_isFocused;

	//Updates
	static void RegisterKeyDown(unsigned char keyIndex);
	static void RegisterKeyUp(unsigned char keyIndex);
	static void RegisterKeyChar(unsigned char keyChar);
	static void BeginFrame();
	static void EndFrame();

	//Queries
	static bool IsKeyDown(unsigned char keyIndex);
	static bool WasKeyJustPressed(unsigned char keyIndex);
	static bool WasKeyJustReleased(unsigned char keyIndex);
	static std::vector<unsigned char> GetAllKeyJustPressed();
	static std::vector<unsigned char> GetAllCharInput();

	//Mouse
	static void SetCursorPosition(const IntVector2& position);
	static IntVector2 GetCursorPosition();
	static Vector2 GetNormalizedCursorPosition(const IntVector2& windowTopLeftPosition, const IntVector2& windowSize);
	static void ShowMouseCursor(bool isNowVisible);

	static void GetInput(std::string& inputStream);

	//Window
	static IntVector2 GetScreenCenter();
	static InputSystem* GetInstance();
	static void OnGainFocus();
	static void OnLostFocus();
	static bool IsFocused();

private:
	static InputSystem* s_instance;

private:
	KeyState	m_areKeyPressed[NUM_KEYBOARD_KEYS];
};