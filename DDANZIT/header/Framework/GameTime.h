#pragma once

#include "GameTimer.h"


class Time
{
    friend class Application;
    friend class Debug;

public:
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize);
    friend void DDANZIT_Run();
    friend void DDANZIT_Finalize();

private:
    static GameTimer* pGameTimer;
    static float fFrameCount;
    static float timeScale;

private:
    void Init();
    void Finalize();

    void Tick();

    static void Pause();
    static void Resume();

public:
    static float unscaledDeltaTime();
    static float deltaTime();
    static float fixedDeltaTime();
};

