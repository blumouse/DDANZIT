#pragma once

#include <vector>
#include <queue>

class GameObject;
class Transform;
class IDrawable;


// 이럴거면 이걸 씬으로 하지말고 따로 빼내는게..?
// Scene에 몰래 끼워들어가는 뭔가 느낌으로
class Scene
{
public:
    friend class GameObject;
    friend class Transform;
    friend class IDrawable;     //? 이건 전처리기로 걸러야겠군.. 아니 뭐 걍 다 써놔도 되긴하겠지만

#pragma region Hierarchy

protected:
    std::vector<Transform*> rootList;
public:
    std::vector<GameObject*> GetRootGameObjects() const;
    const int& rootCount() const;

    //std::vector<Transform*> hierarchy;

#pragma endregion



#pragma region ObjectManagement

private:
    std::queue<GameObject*> gameObjectUpdateList;
    std::queue<IDrawable*> drawableRenderList;

    void RegisterGameObject(GameObject* gameObject);
    void QuitGameObject(GameObject* gameObject);

    void RegisterDrawable(IDrawable* drawable);
    void RegisterDrawable(IDrawable* drawable, int layer);
    void QuitDrawable(IDrawable* drawable);
    void QuitDrawable(IDrawable* drawable, int layer);

#pragma endregion

};

