#pragma once
#include <stdio.h>
//#include <Windows.h>
#include "Engine/Core/Win32Include.hpp"
#include <Xinput.h> // include the Xinput API
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; using 9_1_0 explicitly

#include "Engine/Input/InputSystem.hpp"
#include "../Math/Vector2.hpp"

const int NUM_XBOX_BUTTON = 16;

enum ButtonCode // original value = power(2, BUTTONCODE)
{
	BUTTONCODE_PAD_UP = 0,
	BUTTONCODE_PAD_DOWN = 1,
	BUTTONCODE_PAD_LEFT = 2,
	BUTTONCODE_PAD_RIGHT = 3,
	BUTTONCODE_START = 4,
	BUTTONCODE_BACK = 5,
	BUTTONCODE_LEFT_STICK = 6,
	BUTTONCODE_RIGHT_STICK = 7,
	BUTTONCODE_LB = 8,
	BUTTONCODE_RB = 9,

	BUTTONCODE_LT = 10,
	BUTTONCODE_RT = 11,

	BUTTONCODE_A = 12,
	BUTTONCODE_B = 13,
	BUTTONCODE_X = 14,
	BUTTONCODE_Y = 15,
};

struct ButtonState
{
	bool m_isDown;
	bool m_justChanged;
};

class XboxInput
{
public:
	XINPUT_STATE xboxControllerState;
	DWORD errorStatus;

public:
	XboxInput();
	~XboxInput();

	//Updates
	void InitializeStates();
	void BeginFrame();
	void EndFrame();
	void Render() const;
	void UpdateButtonStatus();
	void UpdateStickStatus();
	void UpdateTriggerStatus();
	void RegisterButtonDown(WORD buttonCode);
	void RegisterButtonUp(WORD buttonCode);
	void CalcStickValue(float& outX, float& outY, SHORT stickValueX, SHORT stickValueY);

	//Queries
	bool IsButtonDown(WORD buttonCode);
	bool WasButtonJustPressed(WORD buttonCode);
	bool WasButtonJustReleased(WORD buttonCode);
	void GetLeftStickStatus(float& x, float& y);
	void GetRightStickStatus(float& x, float& y);
	void GetTriggerStatus(float& left, float& right);
	float GetLeftStickDegrees();
	float GetRightStickDegrees();
	float GetLeftTriggerStatus();
	float GetRightTriggerStatus();
	Vector2 GetLeftStickStatus();
	Vector2 GetRightStickStatus();
	Vector2 GetTriggerStatus();
	float GetLeftStickMagnitude();
	float GetRightStickMagnitude();

	static void StaticBeginFrame();
	static void StaticEndFrame();

	static XboxInput s_controllers[4];
	static XboxInput* GetController(uint8_t index);

private:
	ButtonState m_buttonState[16];
	float m_leftStickX;
	float m_leftStickY;
	float m_rightStickX;
	float m_rightStickY;
	float m_leftTrigger;
	float m_rightTrigger;
};