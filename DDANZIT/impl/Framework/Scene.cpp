#include "Scene.h"

#include "DefineOption.h"

#include "DDANZIT_Core.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Transform.h"

using namespace std;


#pragma region Constructor

Scene::Scene(const string& name) : _name(name), _backgroundColor(Color{1.0f, 1.0f, 1.0f, 1.0f}), isLoaded(false), hierarchy(this)
{

}

#pragma endregion



#pragma region Properties

bool Scene::isSubScene() const 
{
    return SceneManager::GetActiveScene() != this;
}


Color& Scene::backgroundColor()
{
    return _backgroundColor;
}

const Color& Scene::backgroundColor() const
{
    return _backgroundColor;
}

#pragma endregion



#pragma region Hierarchy

vector<GameObject*> Scene::GetRootGameObjects() const 
{
    return hierarchy.pRootGameObjectList;
}

int Scene::rootCount() const 
{
    return hierarchy.pRootGameObjectList.size();
}


void Scene::AddToHierarchy(GameObject* go, Transform* parent) 
{
    go->SetSceneAndDetach(this);

    // 대상이 루트면 할 필요도 없고 하면 안됨 (셋파랜트 내부적으로도 루트에 추가되기 때문)
    if (parent != HIERARCY_ROOT)
        go->_transform->SetParent(parent);

    hierarchy += go;

}

void Scene::RemoveFromHierarchy(GameObject* go) 
{
    // 루트지정 해놓은다음 하이라키에선 지워버려서 고아로 만듬 내부용이라 제한없이 작동 (아마)
    go->_transform->SetParent(HIERARCY_ROOT);
    hierarchy -= go;
}

#pragma endregion


#pragma region GameObjectManagement

#pragma endregion