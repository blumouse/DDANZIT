#pragma once

#include "INC_Windows.h"
#include "Utillity.h"

#include <string>
#include <bitset>


// 이외거는 직접 캐스팅해서 쓰세요 -> (KeyCode)VK_어쩌구
enum class KeyCode
{
    Mouse0 = VK_LBUTTON,
    Mouse1 = VK_RBUTTON,
    Escape = VK_ESCAPE,
    Space = VK_SPACE,
    UpArrow = VK_UP,
    LeftArrow = VK_LEFT,
    DownArrow = VK_DOWN,
    RightArrow = VK_RIGHT,
    W = 'W',
    A = 'A',
    S = 'S',
    D = 'D',
    Q = 'Q',
    E = 'E',
    R = 'R',
    F = 'F',
    Z = 'Z',
    X = 'X',
    C = 'C',
    V = 'V',
    Num1 = '1',
    Num2 = '2',
    Num3 = '3',
    Num4 = '4',
    Num5 = '5',
    Num6 = '6',
    Num7 = '7',
    Num8 = '8',
    Num9 = '9',
    Num0 = '0',
};

struct Gamepad {
    bool isConnected;
    WORD buttons;

    bool abutton;
    bool bbutton;
    bool xbutton;
    bool ybutton;

    bool buttonEast;
    bool buttonWest;
    bool buttonNorth;
    bool buttonSouth;

    bool leftShoulder;
    bool rightShoulder;
    bool leftStickButton;
    bool rightStickButton;
    bool startButton;
    bool selectButton;

    float leftTrigger;
    float rightTrigger;
    float leftStickX, leftStickY;
    float rightStickX, rightStickY;
};

// 레거시 스타일로 처리합시다
class Input
{
public:
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend void DDANZIT_Run();
	friend void DDANZIT_Finalize();


private:
    //static Vector2 prevMousePosition;
    static Vector2 curMousePosition;

    static int curPositionX;
    static int curPositionY;

    static std::bitset<256> isKeyDown;
    static std::bitset<256> isKey;
    static std::bitset<256> isKeyUp;

public:
    static Gamepad pad1;
    static Gamepad pad2;


	// 윈도우 단 내부 콜백들..
private:
    void _OnMouseMove(int x, int y);

    void _OnKeyDown(KeyCode keyCode);
    void _OnKeyUp(KeyCode keyCode);

    void Tick();

    void UpdateGamepad();

public:
    static Vector2 mousePosition();

	static bool GetKeyDown(KeyCode keyCode);
    static bool GetKey(KeyCode keyCode);
    static bool GetKeyUp(KeyCode keyCode);

    static Vector2 GetAxis(const std::string& axisName);
    static Vector2 GetAxisRaw(const std::string& axisName);

};
