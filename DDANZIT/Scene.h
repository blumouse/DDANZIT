#pragma once

#include <vector>
#include <queue>

class GameObject;
class IDrawable;
class Transform;


class Scene
{
private:
    std::queue<GameObject*> pGameObjectList;
    std::queue<IDrawable*> pDrawableList;

    std::vector<Transform*> hierarchy;      // 비스무리하게.. ㅎ 아님 여기를 루트노드로 해도되겠다


public:

    void RegisterObject(GameObject* gameObject);

    void RegisterDrawable(IDrawable* drawable);
    void RegisterDrawable(IDrawable* drawable, int layer);
    void QuitDrawable(IDrawable* drawable);
    void QuitDrawable(IDrawable* drawable, int layer);

    void Destroy(GameObject* gameObject);
};

