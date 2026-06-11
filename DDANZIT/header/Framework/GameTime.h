#pragma once

#include "GameTimer.h"


class Time
{
public:
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize);
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** spritefilePath, unsigned int spriteCount, const wchar_t** mp3filePath, unsigned int mp3Count, const wchar_t** mp4filePath, unsigned int mp4Count, const wchar_t** sfxfilePath, unsigned int sfxCount);

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

