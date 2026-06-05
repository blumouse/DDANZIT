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

    // 폐기
//public:
//    template <std::derived_from<GameObject> T>
//    T* AddGameObject();
//
//    GameObject* AddGameObject();

#pragma endregion

};