#include "DDANZIT_Core.h"

#include "Scene.h"
#include "Lifecycle.h"
#include "Component.h"
#include "GameObject.h"

using namespace std;


// 자료구조로 반영, 별도의 Awake / Start 큐에 등록 및 호출
void DDANZIT_Core::_Awake()
{
    for (Scene* scene : pLoadedSceneList)
    {
        while (!scene->awakeExecQueue.empty())
        {
            scene->awakeExecQueue.front()->Awake();
            scene->awakeExecQueue.pop();
        }
    }
}

void DDANZIT_Core::_OnEnable()
{
    for (Scene* scene : pLoadedSceneList)
    {
        while (!scene->onEnableExecQueue.empty())
        {
            scene->onEnableExecQueue.front()->OnEnable();
            scene->onEnableExecQueue.pop();
        }
    }
}

void DDANZIT_Core::_Start()
{
    for (Scene* scene : pLoadedSceneList)
    {
        while (!scene->startExecQueue.empty())
        {
            scene->startExecQueue.front()->Start();
            scene->startExecQueue.pop();
        }
    }
}


void DDANZIT_Core::_FixedUpdate()
{
    for (Scene* scene : pLoadedSceneList)
    {
        for (Lifecycle* l : scene->fixedUpdateExecList)
            l->FixedUpdate();
    }
}

void DDANZIT_Core::_Update()
{
    for (Scene* scene : pLoadedSceneList)
    {
        for (Lifecycle* l : scene->updateExecList)
            l->Update();
    }
}

void DDANZIT_Core::_LateUpdate()
{
    for (Scene* scene : pLoadedSceneList)
    {
        for (Lifecycle* l : scene->lateUpdateExecList)
            l->LateUpdate();
    }
}


void DDANZIT_Core::_OnDisable()
{
    for (Scene* scene : pLoadedSceneList)
    {
        while (!scene->onDisableExecQueue.empty())
        {
            scene->onDisableExecQueue.front()->OnDisable();
            scene->onDisableExecQueue.pop();
        }
    }
}

void DDANZIT_Core::_OnDestroy()
{
    for (Scene* scene : pLoadedSceneList)
    {
        while (!scene->onDestroyExecQueue.empty())
        {
            scene->onDestroyExecQueue.front()->OnDestroy();

            destroyScheduledQueue.push(dynamic_cast<Component*>(scene->onDestroyExecQueue.front())->gameObject());

            scene->onDestroyExecQueue.pop();
        }
    }
}