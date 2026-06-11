#include "GameTime.h"


GameTimer* Time::pGameTimer = nullptr;
float Time::fFrameCount;


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


float Time::deltaTime()
{
    return pGameTimer->DeltaTimeMS();
}