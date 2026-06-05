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
		// DEBUG: nullptr
		return *this;
	}

	if (go->ownerHierarchy == this)
	{
		// DEBUG: 리스트 안에 이미 주소가 있으면 안돼
		return *this;
	}


	pGameObjectList.push_back(go);
	go->ownerHierarchy = this;

	if (go->transform()->parent() == HIERARCY_ROOT)
		pRootGameObjectList.push_back(go);


	return *this;
}

Hierarchy& Hierarchy::operator-=(GameObject* go) 
{
	if (go == nullptr)
	{
		// DEBUG: nullptr
		return *this;
	}

	if (go->ownerHierarchy != this)
	{
		// DEBUG: 리스트 안에 주소가 없으면 안돼
		return *this;
	}


	pGameObjectList.erase(remove(
		pGameObjectList.begin(),
		pGameObjectList.end(), go),
		pGameObjectList.end());

	go->ownerHierarchy = nullptr;

	if (go->transform()->parent() == HIERARCY_ROOT)
		pRootGameObjectList.erase(remove(
			pRootGameObjectList.begin(),
			pRootGameObjectList.end(), go),
			pRootGameObjectList.end());


	return *this;
}


GameObject* Hierarchy::AddGameObject()
{
	if (pGameObjectList.size() == MAX_SCENE_GAME_OBJECT_NUM)
	{
		// DEBUG: 디버그 메세지
		return nullptr;
	}

	GameObject* go = new GameObject(scene);


	pGameObjectList.push_back(go);
	go->ownerHierarchy = this;

	if (go->transform()->parent() == HIERARCY_ROOT)
		pRootGameObjectList.push_back(go);

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

GameObject* Hierarchy::GetObjectByName(string name)
{
	for (GameObject* go : pGameObjectList)
	{
		if (go->_name == name)
			return go;
	}

	return nullptr;
}