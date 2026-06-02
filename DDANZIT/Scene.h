#pragma once

#include <vector>
#include <queue>

class DDANZIT_Core;
class GameObject;
class IDrawable;
class Lifecycle;
class MonoBehavior;


// 이럴거면 이걸 씬으로 하지말고 따로 빼내는게..?
// Scene에 몰래 끼워들어가는 뭔가 느낌으로
class Scene
{
public:
    friend class DDANZIT_Core;
    friend class GameObject;
    friend class MonoBehavior;
    friend class IDrawable;     // TODO: ? 이건 전처리기로 걸러야겠군.. 아니 뭐 걍 다 써놔도 되긴하겠지만


    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
    friend void DDANZIT_Run();
    friend void DDANZIT_Finalize();

#pragma region Hierarchy

protected:
    std::vector<GameObject*> pRootGameObjectList;
public:
    std::vector<GameObject*> GetRootGameObjects() const;
    const int& rootCount() const;

    //std::vector<Transform*> hierarchy;
    // TODO: 하이라키 타입정의해서 오퍼레이터 +=로 받기 (인스턴트랑 동일한 동작)
    // scene.hierarchy += gameObject; 식

#pragma endregion



#pragma region Lifecycle

    // 이걸 통째로 가져야되나 함수별로 분리해야되나;
    // 한번만 실행이면 큐(삭제되니깐), 계속실행이면 벡터
private:
    std::queue<Lifecycle*> awakeExecQueue;
    std::queue<Lifecycle*> onEnableExecQueue;
    std::queue<Lifecycle*> startExecQueue;

    std::vector<Lifecycle*> fixedUpdateExecList;
    std::vector<Lifecycle*> updateExecList;
    std::vector<Lifecycle*> lateUpdateExecList;

    std::queue<Lifecycle*> onDisableExecQueue;
    std::queue<Lifecycle*> onDestroyExecQueue;

#pragma endregion



#pragma region RenderManagement

private:
    // TODO: 레이어(depth) 적용해서 만들기
    std::queue<IDrawable*> drawableRenderQueue;

    // 이걸 하이라키 등록(오퍼레이터)으로 대체하겠지
    void RegisterGameObject(GameObject* gameObject);
    void QuitGameObject(GameObject* gameObject);

    void RegisterDrawable(IDrawable* drawable);
    void RegisterDrawable(IDrawable* drawable, int layer);
    void QuitDrawable(IDrawable* drawable);
    void QuitDrawable(IDrawable* drawable, int layer);

#pragma endregion

};

