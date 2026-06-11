#include "Input.h"

#include <algorithm>
#include "Camera.h"
#include "DDANZIT_Core.h"

using namespace std;


//Vector2 Input::prevMousePosition = Vector2(0.0f, 0.0f);
Vector2 Input::curMousePosition = Vector2(0.0f, 0.0f);

int Input::curPositionX = 0;
int Input::curPositionY = 0;

bitset<256> Input::isKeyDown;
bitset<256> Input::isKey;
bitset<256> Input::isKeyUp;


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


Vector2 Input::GetAxis(const std::string& axisName)
{
	Vector2 axisVector = Vector2(0.0f, 0.0f);

	if (isKey.test(static_cast<size_t>(KeyCode::UpArrow)))
		axisVector += Vector2(0.0f, 1.0f);
	if (isKey.test(static_cast<size_t>(KeyCode::LeftArrow)))
		axisVector += Vector2(-1.0f, 0.0f);
	if (isKey.test(static_cast<size_t>(KeyCode::DownArrow)))
		axisVector += Vector2(0.0f, -1.0f);
	if (isKey.test(static_cast<size_t>(KeyCode::RightArrow)))
		axisVector += Vector2(1.0f, 0.0f);

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
		// DEBUG: 그없
		return Vector2(0.0f, 0.0f);
	}
}

Vector2 Input::GetAxisRaw(const std::string& axisName)
{
	Vector2 axisVector = Vector2(0.0f, 0.0f);

	if (isKey.test(static_cast<size_t>(KeyCode::UpArrow)))
		axisVector += Vector2(0.0f, 1.0f);
	if (isKey.test(static_cast<size_t>(KeyCode::LeftArrow)))
		axisVector += Vector2(-1.0f, 0.0f);
	if (isKey.test(static_cast<size_t>(KeyCode::DownArrow)))
		axisVector += Vector2(0.0f, -1.0f);
	if (isKey.test(static_cast<size_t>(KeyCode::RightArrow)))
		axisVector += Vector2(1.0f, 0.0f);


	if (axisName == "Horizontal")
	{
		axisVector.x = 0.0f;
		axisVector.y = clamp(axisVector.y, -1.0f, 1.0f);
		return axisVector;
	}
	else if (axisName == "Vertical")
	{
		axisVector.y = 0.0f;
		axisVector.x = clamp(axisVector.x, -1.0f, 1.0f);
		return axisVector;
	}
	else
	{
		// DEBUG: 그없
		return Vector2(0.0f, 0.0f);
	}
}
