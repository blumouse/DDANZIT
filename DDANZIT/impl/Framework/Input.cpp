#include "Input.h"

#include "Xinput.h"
#pragma comment(lib, "Xinput.lib")

#include <algorithm>
#include "Camera.h"
#include "DDANZIT_Core.h"

#include "Debug.h"

using namespace std;


//Vector2 Input::prevMousePosition = Vector2(0.0f, 0.0f);
Vector2 Input::curMousePosition = Vector2(0.0f, 0.0f);

int Input::curPositionX = 0;
int Input::curPositionY = 0;

bitset<256> Input::isKeyDown;
bitset<256> Input::isKey;
bitset<256> Input::isKeyUp;

Gamepad Input::pad1;
Gamepad Input::pad2;


void Input::_OnMouseMove(int x, int y)
{
	curPositionX = x;
	curPositionY = y;
}


void Input::_OnKeyDown(KeyCode keyCode)
{
	isKeyDown.set(static_cast<size_t>(keyCode));
	isKey.set(static_cast<size_t>(keyCode));
}

void Input::_OnKeyUp(KeyCode keyCode)
{
	isKey.reset(static_cast<size_t>(keyCode));
	isKeyUp.set(static_cast<size_t>(keyCode));
}


void Input::Tick()
{
	isKeyDown.reset();
	isKeyUp.reset();
}


void ClampDeadZoneStick(bool isR, float& stickX, float& stickY)
{
	float thumbLX = stickX;
	float thumbLY = stickY;

	float magnitude = std::sqrt(thumbLX * thumbLX + thumbLY * thumbLY);

	float normalizedLX = thumbLX / magnitude;
	float normalizedLY = thumbLY / magnitude;

	float deadzone;
	if (!isR)
		deadzone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	else
		deadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

	if (magnitude > deadzone) 
	{
		if (magnitude > 32767.0f) magnitude = 32767.0f;

		magnitude = (magnitude - deadzone) / (32767.0f - deadzone);

		stickX = normalizedLX * magnitude;
		stickY = normalizedLY * magnitude;
	}
	else 
	{
		stickX = 0.0f;
		stickY = 0.0f;
	}
}

void ClampDeadZoneTrigger(bool isR, float& trigger)
{
	float rawT = trigger;

	if (rawT > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		trigger = (rawT - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (255.0f - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	else 
		trigger = 0.0f;
}

void Input::UpdateGamepad()
{
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	// 0번은 첫 번째 플레이어 컨트롤러
	DWORD result = XInputGetState(0, &state);

	if (result == ERROR_SUCCESS) 
	{
		pad1.isConnected = true;

		pad1.buttons = state.Gamepad.wButtons;

		pad1.abutton = pad1.buttons & XINPUT_GAMEPAD_A;
		pad1.bbutton = pad1.buttons & XINPUT_GAMEPAD_B;
		pad1.xbutton = pad1.buttons & XINPUT_GAMEPAD_X;
		pad1.ybutton = pad1.buttons & XINPUT_GAMEPAD_Y;

		pad1.buttonEast = pad1.buttons & XINPUT_GAMEPAD_DPAD_RIGHT;
		pad1.buttonWest = pad1.buttons & XINPUT_GAMEPAD_DPAD_LEFT;
		pad1.buttonNorth = pad1.buttons & XINPUT_GAMEPAD_DPAD_UP;
		pad1.buttonSouth = pad1.buttons & XINPUT_GAMEPAD_DPAD_DOWN;

		pad1.leftShoulder = pad1.buttons & XINPUT_GAMEPAD_LEFT_SHOULDER;
		pad1.rightShoulder = pad1.buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
		pad1.leftStickButton = pad1.buttons & XINPUT_GAMEPAD_LEFT_THUMB;
		pad1.rightStickButton = pad1.buttons & XINPUT_GAMEPAD_RIGHT_THUMB;
		pad1.startButton = pad1.buttons & XINPUT_GAMEPAD_START;
		pad1.selectButton = pad1.buttons & XINPUT_GAMEPAD_BACK;


		pad1.leftTrigger = state.Gamepad.bLeftTrigger;		ClampDeadZoneTrigger(false, pad1.leftTrigger);
		pad1.rightTrigger = state.Gamepad.bRightTrigger;	ClampDeadZoneTrigger(true, pad1.rightTrigger);

		pad1.leftStickX = state.Gamepad.sThumbLX;
		pad1.leftStickY = state.Gamepad.sThumbLY;
		ClampDeadZoneStick(false, pad1.leftStickX, pad1.leftStickY);

		pad1.rightStickX = state.Gamepad.sThumbRX;
		pad1.rightStickY = state.Gamepad.sThumbRY;
		ClampDeadZoneStick(true, pad1.rightStickX, pad1.rightStickY);
	}
	else 
	{
		pad1.isConnected = false; // 컨트롤러 연결 끊김
	}


	ZeroMemory(&state, sizeof(XINPUT_STATE));
	result = XInputGetState(1, &state);

	if (result == ERROR_SUCCESS)
	{
		pad2.isConnected = true;

		pad2.buttons = state.Gamepad.wButtons;

		pad2.abutton = pad2.buttons & XINPUT_GAMEPAD_A;
		pad2.bbutton = pad2.buttons & XINPUT_GAMEPAD_B;
		pad2.xbutton = pad2.buttons & XINPUT_GAMEPAD_X;
		pad2.ybutton = pad2.buttons & XINPUT_GAMEPAD_Y;

		pad2.buttonEast = pad2.buttons & XINPUT_GAMEPAD_DPAD_RIGHT;
		pad2.buttonWest = pad2.buttons & XINPUT_GAMEPAD_DPAD_LEFT;
		pad2.buttonNorth = pad2.buttons & XINPUT_GAMEPAD_DPAD_UP;
		pad2.buttonSouth = pad2.buttons & XINPUT_GAMEPAD_DPAD_DOWN;

		pad2.leftShoulder = pad2.buttons & XINPUT_GAMEPAD_LEFT_SHOULDER;
		pad2.rightShoulder = pad2.buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
		pad2.leftStickButton = pad2.buttons & XINPUT_GAMEPAD_LEFT_THUMB;
		pad2.rightStickButton = pad2.buttons & XINPUT_GAMEPAD_RIGHT_THUMB;
		pad2.startButton = pad2.buttons & XINPUT_GAMEPAD_START;
		pad2.selectButton = pad2.buttons & XINPUT_GAMEPAD_BACK;


		pad2.leftTrigger = state.Gamepad.bLeftTrigger;		ClampDeadZoneTrigger(false, pad2.leftTrigger);
		pad2.rightTrigger = state.Gamepad.bRightTrigger;	ClampDeadZoneTrigger(true, pad2.rightTrigger);

		pad2.leftStickX = state.Gamepad.sThumbLX;
		pad2.leftStickY = state.Gamepad.sThumbLY;
		ClampDeadZoneStick(false, pad2.leftStickX, pad2.leftStickY);

		pad2.rightStickX = state.Gamepad.sThumbRX;
		pad2.rightStickY = state.Gamepad.sThumbRY;
		ClampDeadZoneStick(true, pad2.rightStickX, pad2.rightStickY);
	}
	else
	{
		pad2.isConnected = false;
	}
}


Vector2 Input::mousePosition()
{
	return Vector2(
		(float)(curPositionX - DDANZIT_Core::width) / Camera::worldToScreenRatio,
		(float)(curPositionY - DDANZIT_Core::height) / Camera::worldToScreenRatio);
}


bool Input::GetKeyDown(KeyCode keyCode)
{
	return isKeyDown.test(static_cast<size_t>(keyCode));
}

bool Input::GetKey(KeyCode keyCode)
{
	return isKey.test(static_cast<size_t>(keyCode));
}

bool Input::GetKeyUp(KeyCode keyCode)
{
	return isKeyUp.test(static_cast<size_t>(keyCode));
}

// ??? 왜안되지......
Vector2 Input::GetAxis(const std::string& axisName)
{
	Vector2 axisVector = Vector2(0.0f, 0.0f);

	if (GetKey(KeyCode::UpArrow) || GetKey(KeyCode::W))
		axisVector.y += 1.0f;
	if (GetKey(KeyCode::DownArrow) || GetKey(KeyCode::S))
		axisVector.y += -1.0f;
	if (GetKey(KeyCode::LeftArrow) || GetKey(KeyCode::A))
		axisVector.x += -1.0f;
	if (GetKey(KeyCode::RightArrow) || GetKey(KeyCode::D))
		axisVector.x += 1.0f;

	if (pad1.isConnected)
		axisVector += Vector2(pad1.leftStickX, pad1.leftStickY);


	if (axisVector.x * axisVector.x + axisVector.y * axisVector.y > 1.0f)
		axisVector.Normalize();

	// 귀찮으니까 하드코딩함
	if (axisName == "Horizontal")
	{
		axisVector.x = 0.0f;
		return axisVector;
	}
	else if (axisName == "Vertical")
	{
		axisVector.y = 0.0f;
		return axisVector;
	}
	else
	{
		Debug::Log("GetAxis: 존재하지 않는 Axis 이름입니다.");
		return Vector2(0.0f, 0.0f);
	}
}

Vector2 Input::GetAxisRaw(const std::string& axisName)
{
	Vector2 axisVector = Vector2(0.0f, 0.0f);

	if (axisName == "Horizontal")
	{
		if (GetKey(KeyCode::UpArrow) || GetKey(KeyCode::W))
			axisVector.y += 1.0f;
		if (GetKey(KeyCode::DownArrow) || GetKey(KeyCode::S))
			axisVector.y += -1.0f;

		if (pad1.isConnected)
			axisVector.y += pad1.leftStickY;

		//axisVector.y = clamp(axisVector.y, -1.0f, 1.0f);
		return axisVector;
	}
	else if (axisName == "Vertical")
	{
		if (GetKey(KeyCode::LeftArrow) || GetKey(KeyCode::A))
			axisVector.x += -1.0f;
		if (GetKey(KeyCode::RightArrow) || GetKey(KeyCode::D))
			axisVector.x += 1.0f;

		if (pad1.isConnected)
			axisVector.x += pad1.leftStickX;

		//axisVector.x = clamp(axisVector.x, -1.0f, 1.0f);
		return axisVector;
	}
	else
	{
		Debug::Log("GetAxis: 존재하지 않는 Axis 이름입니다.");
		return Vector2(0.0f, 0.0f);
	}
}
