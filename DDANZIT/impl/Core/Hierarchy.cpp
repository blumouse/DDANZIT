#include "Hierarchy.h"

#include "Debug.h"

#include "SceneManager.h"
#include "Scene.h"

#include "GameObject.h"
#include "Transform.h"

using namespace std;


#pragma region Constructor

Hierarchy::Hierarchy(Scene* scene) : scene(scene)
{

}

#pragma endregion



Hierarchy& Hierarchy::operator+=(GameObject* go) 
{
	if (go == nullptr) 
	{
		Debug::Assert(false, "Hierarchy: 오브젝트가 nullptr입니다.");
		return *this;
	}

	if (go->ownerHierarchy == this)
	{
		Debug::Log("Hierarchy: 이미 존재하는 오브젝트입니다. : " + go->_name);
		return *this;
	}


	pGameObjectList.push_back(go);
	go->ownerHierarchy = this;

	if (go->_transform->parent() == HIERARCY_ROOT)
		pRootGameObjectList.push_back(go);

#ifdef USE_DEBUG_TUI
	Debug::ChangedHierarchyInfo();

#endif // USE_DEBUG_TUI


	return *this;
}

Hierarchy& Hierarchy::operator-=(GameObject* go) 
{
	if (go == nullptr)
	{
		Debug::Assert(false, "Hierarchy: 오브젝트가 nullptr입니다.");
		return *this;
	}

	if (go->ownerHierarchy != this)
	{
		Debug::Log("Hierarchy: 하이라키에 존재하지 않는 오브젝트입니다. : " + go->_name);
		return *this;
	}


	pGameObjectList.erase(remove(
		pGameObjectList.begin(),
		pGameObjectList.end(), go),
		pGameObjectList.end());

	go->ownerHierarchy = nullptr;

	if (go->_transform->parent() == HIERARCY_ROOT)
		pRootGameObjectList.erase(remove(
			pRootGameObjectList.begin(),
			pRootGameObjectList.end(), go),
			pRootGameObjectList.end());

#ifdef USE_DEBUG_TUI
	Debug::ChangedHierarchyInfo();

#endif // USE_DEBUG_TUI


	return *this;
}


GameObject* Hierarchy::AddGameObject()
{
	if (pGameObjectList.size() == MAX_SCENE_GAME_OBJECT_NUM)
	{
		Debug::Log("AddGameObject: 오브젝트 수가 최대입니다.");
		return nullptr;
	}

	GameObject* go = new GameObject(scene);

	go->isInitialized = true;


	*this += go;

	return go;
}


GameObject* Hierarchy::AddGameObject(const string& name)
{
	if (pGameObjectList.size() == MAX_SCENE_GAME_OBJECT_NUM)
	{
		Debug::Log("AddGameObject: 오브젝트 수가 최대입니다.");
		return nullptr;
	}

	GameObject* go = new GameObject(scene);
	go->_name = name;

	go->isInitialized = true;


	*this += go;

	return go;
}

//void Hierarchy::QuitGameObject(GameObject* go)
//{
//
//}


vector<GameObject*> Hierarchy::GetObjectList()
{
	return pGameObjectList;
}

GameObject* Hierarchy::GetObjectByTag(Tag tag)
{
	for (GameObject* go : pGameObjectList)
	{
		if (go->_tag == tag)
			return go;
	}

	return nullptr;
}

GameObject* Hierarchy::GetObjectByName(const std::string& name)
{
	for (GameObject* go : pGameObjectList)
	{
		if (go->_name == name)
			return go;
	}

	return nullptr;
}