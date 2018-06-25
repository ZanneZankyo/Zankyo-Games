#include "Engine/Input/XboxInput.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vector2.hpp"

XboxInput XboxInput::s_controllers[4];

XboxInput::XboxInput()
	:m_leftStickX(0)
	,m_leftStickY(0)
	,m_rightStickX(0)
	,m_rightStickY(0)
	,m_leftTrigger(0)
	,m_rightTrigger(0)
{
	/*if (errorStatus == ERROR_SUCCESS)
	{
		printf("  wButtons=%5d (0x%08x), bLeftTrigger=%3d, bRightTrigger=%3d\n",
			xboxControllerState.Gamepad.wButtons, xboxControllerState.Gamepad.wButtons,
			xboxControllerState.Gamepad.bLeftTrigger, xboxControllerState.Gamepad.bRightTrigger);
		printf("  sThumbLX=%d\n", xboxControllerState.Gamepad.sThumbLX);
		printf("  sThumbLY=%d\n", xboxControllerState.Gamepad.sThumbLY);
		printf("  sThumbRX=%d\n", xboxControllerState.Gamepad.sThumbRX);
		printf("  sThumbRY=%d\n", xboxControllerState.Gamepad.sThumbRY);
	}*/
	InitializeStates();
}

XboxInput::~XboxInput()
{
}

void XboxInput::InitializeStates()
{
	for (int buttonCode = 0; buttonCode < NUM_XBOX_BUTTON; buttonCode++)
	{
		m_buttonState[buttonCode].m_isDown = false;
		m_buttonState[buttonCode].m_justChanged = false;
	}
	m_leftStickX = 0;
	m_leftStickY = 0;
	m_rightStickX = 0;
	m_rightStickY = 0;
	m_leftTrigger = 0;
	m_rightTrigger = 0;
}

void XboxInput::BeginFrame()
{
	memset(&xboxControllerState, 0, sizeof(xboxControllerState));
	errorStatus = XInputGetState(0, &xboxControllerState);
	if (errorStatus == ERROR_SUCCESS)
	{
		UpdateButtonStatus();
		UpdateStickStatus();
		UpdateTriggerStatus();
	}
		
	else
		InitializeStates();
}

void XboxInput::EndFrame()
{
	for (int buttonCode = 0; buttonCode < NUM_XBOX_BUTTON; buttonCode++)
	{
		m_buttonState[buttonCode].m_justChanged = false;
	}
}

void XboxInput::Render() const
{
}

//Button status is stored in a 16 bits variable, each digit represent a button status
void XboxInput::UpdateButtonStatus()
{
	WORD buttons = xboxControllerState.Gamepad.wButtons;
	for (WORD buttonCode = 0; buttonCode < NUM_XBOX_BUTTON; buttonCode++)
	{
		if (buttons % 2 == 1 && buttonCode != BUTTONCODE_LT&&buttonCode != BUTTONCODE_RT)
			RegisterButtonDown(buttonCode);
		else if (buttonCode != BUTTONCODE_LT&&buttonCode != BUTTONCODE_RT)
			RegisterButtonUp(buttonCode);
		buttons >>= 1;
		/*if (buttons & buttonCode && buttonCode != BUTTONCODE_LT&&buttonCode != BUTTONCODE_RT)
			RegisterButtonDown(buttonCode);
		else if(buttonCode != BUTTONCODE_LT&&buttonCode != BUTTONCODE_RT)
			RegisterButtonUp(buttonCode);*/
	}
}

void XboxInput::UpdateStickStatus()
{
	CalcStickValue(m_leftStickX, m_leftStickY,xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY);
	CalcStickValue(m_rightStickX, m_rightStickY,xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);
}

void XboxInput::UpdateTriggerStatus()
{
	float scale = 1 / 255.f;
	m_leftTrigger = xboxControllerState.Gamepad.bLeftTrigger *scale;
	if (m_leftTrigger > 0)
		RegisterButtonDown(BUTTONCODE_LT);
	else
		RegisterButtonUp(BUTTONCODE_LT);

	m_rightTrigger = xboxControllerState.Gamepad.bRightTrigger * scale;
	if (m_rightTrigger > 0)
		RegisterButtonDown(BUTTONCODE_RT);
	else
		RegisterButtonUp(BUTTONCODE_RT);
}

void XboxInput::CalcStickValue(float& outX, float& outY, SHORT stickValueX, SHORT stickValueY)
{
	//DebuggerPrintf("stick:%d,%d", stickValueX, stickValueY);
	Vector2 stickValue(stickValueX, stickValueY);
	float stickLength = stickValue.CalcLength();
	if (-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE < stickLength && stickLength < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		stickValue.SetLength(0.f);
	}
	else if (stickLength < -32767 * 0.95f || 32767 * 0.95f < stickLength)
	{
		stickValue.SetLength(1.f);
	}
	else
	{
		stickValue.SetLength(RangeMap(stickLength, -32767 * 0.95f, 32767 * 0.95f, -1.f, 1.f));
	}
	stickValue.GetXY(outX, outY);
}

void XboxInput::RegisterButtonDown(WORD buttonCode)
{
	bool wasDown = m_buttonState[buttonCode].m_isDown;
	m_buttonState[buttonCode].m_isDown = true;

	if (m_buttonState[buttonCode].m_isDown != wasDown)
		m_buttonState[buttonCode].m_justChanged = true;
	else
		m_buttonState[buttonCode].m_justChanged = false;
}

void XboxInput::RegisterButtonUp(WORD buttonCode)
{
	bool wasDown = m_buttonState[buttonCode].m_isDown;
	m_buttonState[buttonCode].m_isDown = false;

	if (m_buttonState[buttonCode].m_isDown != wasDown)
		m_buttonState[buttonCode].m_justChanged = true;
	else
		m_buttonState[buttonCode].m_justChanged = false;
}

bool XboxInput::IsButtonDown(WORD buttonCode)
{
	return m_buttonState[buttonCode].m_isDown;
}

bool XboxInput::WasButtonJustPressed(WORD buttonCode)
{
	return IsButtonDown(buttonCode) && m_buttonState[buttonCode].m_justChanged;
}

bool XboxInput::WasButtonJustReleased(WORD buttonCode)
{
	return !IsButtonDown(buttonCode) && m_buttonState[buttonCode].m_justChanged;
}

void XboxInput::GetLeftStickStatus(float & x, float & y)
{
	x = m_leftStickX;
	y = m_leftStickY;
}

Vector2 XboxInput::GetLeftStickStatus()
{
	return Vector2(m_leftStickX, m_leftStickY);
}

float XboxInput::GetLeftStickDegrees()
{
	return Atan2Degrees(m_leftStickY, m_leftStickX);
}

void XboxInput::GetRightStickStatus(float & x, float & y)
{
	x = m_rightStickX;
	y = m_rightStickY;
}

Vector2 XboxInput::GetRightStickStatus()
{
	return Vector2(m_rightStickX, m_rightStickY);
}

float XboxInput::GetRightStickDegrees()
{
	return Atan2Degrees(m_rightStickY, m_rightStickX);
}

float XboxInput::GetLeftTriggerStatus()
{
	return m_leftTrigger;
}

float XboxInput::GetRightTriggerStatus()
{
	return m_rightTrigger;
}

void XboxInput::GetTriggerStatus(float & left, float & right)
{
	left = m_leftTrigger;
	right = m_rightTrigger;
}

Vector2 XboxInput::GetTriggerStatus()
{
	return Vector2(m_leftTrigger, m_rightTrigger);
}

float XboxInput::GetLeftStickMagnitude()
{
	return Vector2(m_leftStickX,m_leftStickY).CalcLength();
}

float XboxInput::GetRightStickMagnitude()
{
	return Vector2(m_leftStickX, m_rightStickY).CalcLength();
}

void XboxInput::StaticBeginFrame()
{
	for (int index = 0; index < 4; index++)
		s_controllers[index].BeginFrame();
}

void XboxInput::StaticEndFrame()
{
	for (int index = 0; index < 4; index++)
		s_controllers[index].EndFrame();
}

XboxInput * XboxInput::GetController(uint8_t index)
{
	if (index >= 4)
		return nullptr;
	return &s_controllers[index];
}
