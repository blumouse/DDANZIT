#include "DDANZIT_Core.h"

#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "MonoBehavior.h"

using namespace std;


#pragma region Methods

void DDANZIT_Core::DestroyScheduled()
{
    while (!destroyScheduledQueue.empty())
    {
        GameObject* gameObject = destroyScheduledQueue.front();
        Scene* targetScene = gameObject->_scene;


        // 라이프사이클 함수들 리스트에서 빼주기
        // 이거 위에 저거로 위임하는게 낫겠다 디스트로이에서 넣어주고
        //for (Component* comp : gameObject->pComponentList)
        //{
        //    if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(comp))
        //    {
        //        if (b->activeFixedUpdate)
        //            targetScene->fixedUpdateExecList.erase(remove(
        //                targetScene->fixedUpdateExecList.begin(),
        //                targetScene->fixedUpdateExecList.end(), b),
        //                targetScene->fixedUpdateExecList.end());

        //        if (b->activeUpdate)
        //            targetScene->updateExecList.erase(remove(
        //                targetScene->updateExecList.begin(),
        //                targetScene->updateExecList.end(), b),
        //                targetScene->updateExecList.end());

        //        if (b->activeLateUpdate)
        //            targetScene->lateUpdateExecList.erase(remove(
        //                targetScene->lateUpdateExecList.begin(),
        //                targetScene->lateUpdateExecList.end(), b),
        //                targetScene->lateUpdateExecList.end());
        //    }
        //}


        // 씬 / 하이라키(루트)에서 빼버리기
        // ..하이라키를 아직 안만들었네; 상관은 없지만서도
        if (gameObject->_transform->_parent == nullptr)
        {
            targetScene->RemoveFromHierarchy(gameObject);
        }
        else
        {
            gameObject->_transform->_parent->RemoveChild(gameObject->_transform);	// 별로 좋은 코드는 아니군

            gameObject->_transform->_parent = nullptr;
        }


        // TODO_LATER: (게임아닌)오브젝트를 받는다면... 컴포넌트인 경우의 처리(분기)


        // 삭제!
        delete destroyScheduledQueue.front();
        destroyScheduledQueue.pop();
    }
}

#pragma endregion



#pragma region Lifecycles

void DDANZIT_Core::_Awake()
{
    while (!awakeExecQueue.empty())
    {
        awakeExecQueue.front()->Awake();
        awakeExecQueue.pop();
    }
}

void DDANZIT_Core::_OnEnable()
{
    while (!onEnableExecQueue.empty())
    {
        onEnableExecQueue.front()->OnEnable();
        onEnableExecQueue.pop();
    }
}

void DDANZIT_Core::_Start()
{
    while (!startExecQueue.empty())
    {
        startExecQueue.front()->Start();
        startExecQueue.pop();
    }
}

// 얘네들 비활/파괴면 실행하지 말아야함
// strict-unity니까.. 그렇게 하자고
void DDANZIT_Core::_FixedUpdate()
{
    for (MonoBehavior* b : fixedUpdateExecList)
    {
        if (b->isActiveAndEnabled() && !b->gameObject()->isKilled)
            b->FixedUpdate();
    }
}

void DDANZIT_Core::_Update()
{
    for (MonoBehavior* b : updateExecList)
    {
        if (b->isActiveAndEnabled() && !b->gameObject()->isKilled)
            b->Update();
    }
}

void DDANZIT_Core::_LateUpdate()
{
    for (MonoBehavior* b : lateUpdateExecList)
    {
        if (b->isActiveAndEnabled() && !b->gameObject()->isKilled)
            b->LateUpdate();
    }
}


void DDANZIT_Core::_OnDisable()
{
    while (!onDisableExecQueue.empty())
    {
        onDisableExecQueue.front()->OnDisable();
        onDisableExecQueue.pop();
    }
}

void DDANZIT_Core::_OnDestroy()
{
    while (!onDestroyExecQueue.empty())
    {
        onDestroyExecQueue.front()->OnDestroy();

        destroyScheduledQueue.push(onDestroyExecQueue.front()->gameObject());

        onDestroyExecQueue.pop();
    }
}


void DDANZIT_Core::RegisterUpdateScheduled()
{
    while (!registerUpdateScheduledQueue.empty())
    {
        MonoBehavior* b = registerUpdateScheduledQueue.front();

        if (b->activeFixedUpdate)
            fixedUpdateExecList.push_back(b);

        if (b->activeUpdate)
            updateExecList.push_back(b);

        if (b->activeLateUpdate)
            lateUpdateExecList.push_back(b);

        b->isInUpdateList = true;
        registerUpdateScheduledQueue.pop();
    }
}

void DDANZIT_Core::QuitUpdateScheduled()
{
    while (!quitUpdateScheduledQueue.empty())
    {
        MonoBehavior* b = quitUpdateScheduledQueue.front();

        if (b->activeFixedUpdate)
            fixedUpdateExecList.erase(remove(
                fixedUpdateExecList.begin(),
                fixedUpdateExecList.end(), b),
                fixedUpdateExecList.end());

        if (b->activeUpdate)
            updateExecList.erase(remove(
                updateExecList.begin(),
                updateExecList.end(), b),
                updateExecList.end());

        if (b->activeLateUpdate)
            lateUpdateExecList.erase(remove(
                lateUpdateExecList.begin(),
                lateUpdateExecList.end(), b),
                lateUpdateExecList.end());

        registerUpdateScheduledQueue.pop();
    }
}


void DDANZIT_Core::RegisterUpdateExecLists(MonoBehavior* behavior)
{
    // 오기전에 걸렀어
    //if (behavior->isInUpdateList)
    //    return;

    registerUpdateScheduledQueue.push(behavior);
    behavior->isInUpdateList = true;
}

// 지금은 파괴 시에만 호출
void DDANZIT_Core::QuitUpdateExecLists(MonoBehavior* behavior)
{
    quitUpdateScheduledQueue.push(behavior);
}

#pragma endregion
