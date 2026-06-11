#include "GameTime.h"


GameTimer* Time::pGameTimer = nullptr;
float Time::fFrameCount = 0.0f;
float Time::timeScale = 1.0f;


void Time::Init() 
{
    pGameTimer = new GameTimer();
    pGameTimer->Reset();
    fFrameCount = 0.0f;
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
    return pGameTimer->DeltaTimeMS();
}

float Time::deltaTime()
{
    return pGameTimer->DeltaTimeMS() * timeScale;
}

float Time::fixedDeltaTime()
{
    return 200.0f;
}
