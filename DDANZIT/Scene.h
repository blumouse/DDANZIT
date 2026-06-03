#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <queue>

#include "Hierarchy.h"

class GameObject;
class IDrawable;
class Lifecycle;


class Scene
{
public:
    friend class DDANZIT_Core;
    friend class SceneManager;
    friend class GameObject;
    friend class MonoBehavior;

    friend class IDrawable;     // TODO: ? 이건 전처리기로 걸러야겠군.. 아니 뭐 걍 다 써놔도 되긴하겠지만


    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
    friend void DDANZIT_Run();
    friend void DDANZIT_Finalize();

#pragma region Constructor

    Scene() = delete;
    Scene(std::string name);
    Scene(const Scene&) = delete;

    ~Scene() = default;

#pragma endregion



#pragma region Properties

protected:
    std::string _name;
    std::string& name() { return _name; }
public:
    const std::string& name() const { return _name; }


public:
    bool isSubScene() const;

//public:
//    bool isLoaded() const;

//public:
//    const wchar_t* path() const;

private:
    bool isLoaded;

#pragma endregion



#pragma region Hierarchy

public:
    std::vector<GameObject*> GetRootGameObjects() const;
    const int& rootCount() const;


    // 유사 하이라키
public:
    Hierarchy hierarchy;

    // 내부용 함수들 좀 만들어야겠다
private:
    void AddToHierarchy(GameObject* go, Transform* parent);
    void RemoveFromHierarchy(GameObject* go);

#pragma endregion



#pragma region GameObjectManagement

public:
    // TODO: 굳이 따지면 CreatePrimitive에 해당한다
    // 하이라키에 오브젝트 하나 만들어 올린다
    // 뭔가 더 해야될거 같은데..?
    GameObject* AddGameObject()
    {
        if (hierarchy.pGameObjectList.size() == MAX_SCENE_GAME_OBJECT_NUM)
        {
            // DEBUG: 너무많아
            return;
        }


        GameObject* gameObject = new GameObject(this);

        hierarchy += gameObject;
    }

    // 사용감을 높이기 위한...
    template <std::derived_from<GameObject> T>
    GameObject* AddGameObject() 
    {
        if (hierarchy.pGameObjectList.size() == MAX_SCENE_GAME_OBJECT_NUM)
        {
            // DEBUG: 디버그 메세지
            return;
        }


        T* gameObject = new T(this);

        hierarchy += gameObject;
    }

#pragma endregion

};

