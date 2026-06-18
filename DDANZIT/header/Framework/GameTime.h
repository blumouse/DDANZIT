#pragma once

#include "GameTimer.h"


class Time
{
    friend class Application;
    friend class Debug;

public:
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
    friend void DDANZIT_Run();
    friend void DDANZIT_Finalize();

private:
    static GameTimer* pGameTimer;
    static float frameCount;
    static float fFrameCount;
    static float timeScale;
    static float updateDeltaTimeMs;
    static float updateUnscaledDeltaTimeMs;

private:
    void Init();
    void Finalize();

    void Tick();

    static void Pause();
    static void Resume();

public:
    static float unscaledDeltaTime(); // milliseconds without timeScale
    static float deltaTime();     // milliseconds, legacy engine convention
    static float deltaTimeMs();   // milliseconds
    static float fixedDeltaTime();
};

