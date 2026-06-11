#pragma once

#include "INC_Windows.h"
#include "Utillity.h"
#include <string>
#include <bitset>

using Vector2 = learning::Vector2f;


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

// 레거시 스타일로 처리합시다
class Input
{
public:
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize);
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** spritefilePath, unsigned int spriteCount, const wchar_t** mp3filePath, unsigned int mp3Count, const wchar_t** mp4filePath, unsigned int mp4Count, const wchar_t** sfxfilePath, unsigned int sfxCount);

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


	// 윈도우 단 내부 콜백들..
private:
    void _OnMouseMove(int x, int y);

    void _OnKeyDown(KeyCode keyCode);
    void _OnKeyUp(KeyCode keyCode);

    void Tick();

public:
    static Vector2 mousePosition();

	static bool GetKeyDown(KeyCode keyCode);
    static bool GetKey(KeyCode keyCode);
    static bool GetKeyUp(KeyCode keyCode);

    static Vector2 GetAxis(const std::string& axisName);
    static Vector2 GetAxisRaw(const std::string& axisName);

};
