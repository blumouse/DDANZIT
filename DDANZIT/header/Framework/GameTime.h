#pragma once

#include "GameTimer.h"


class Time
{
public:
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
    friend void DDANZIT_Run();
    friend void DDANZIT_Finalize();

private:
    static GameTimer* pGameTimer;
    static float fFrameCount;

private:
    void Init();
    void Finalize();

    void Tick();

public:
    static float deltaTime();
};

