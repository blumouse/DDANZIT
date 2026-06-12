#pragma once

#include "GameTimer.h"


class Time
{
    friend class Application;
    friend class Debug;

public:
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize);
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** spritefilePath, unsigned int spriteCount, const wchar_t** mp3filePath, unsigned int mp3Count, const wchar_t** mp4filePath, unsigned int mp4Count, const wchar_t** sfxfilePath, unsigned int sfxCount);

    friend void DDANZIT_Run();
    friend void DDANZIT_Finalize();

private:
    static GameTimer* pGameTimer;
    static float frameCount;
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
    static float deltaTime();   // 밀리초니까 1000.0f 나눠서 쓰는게 좋을지도
    static float fixedDeltaTime();
};

