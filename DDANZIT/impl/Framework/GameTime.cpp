#include "GameTime.h"


GameTimer* Time::pGameTimer = nullptr;
float Time::frameCount = 0.0f;
float Time::fFrameCount = 0.0f;
float Time::timeScale = 1.0f;
float Time::updateDeltaTimeMs = 0.0f;
float Time::updateUnscaledDeltaTimeMs = 0.0f;


void Time::Init() 
{
    pGameTimer = new GameTimer();
    pGameTimer->Reset();
    frameCount = 0.0f;
    fFrameCount = 0.0f;
    updateDeltaTimeMs = 0.0f;
    updateUnscaledDeltaTimeMs = 0.0f;
}

void Time::Finalize()
{
    delete pGameTimer;
    pGameTimer = nullptr;
}


void Time::Tick()
{
    pGameTimer->Tick();
}

void Time::Pause()
{
    pGameTimer->Stop();
}

void Time::Resume()
{
    pGameTimer->Start();
}


float Time::unscaledDeltaTime()
{
    return updateUnscaledDeltaTimeMs;
}

float Time::deltaTime()
{
    return updateDeltaTimeMs;
}

float Time::deltaTimeMs()
{
    return updateDeltaTimeMs;
}

float Time::fixedDeltaTime()
{
    return 20.0f;
}
