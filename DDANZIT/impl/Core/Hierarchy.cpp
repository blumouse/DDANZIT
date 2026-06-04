#include "Hierarchy.h"

#include "DefineOption.h"
#include "Debug.h"

#include "SceneManager.h"
#include "GameObject.h"
#include "Transform.h"

using namespace std;


Hierarchy& Hierarchy::operator+=(GameObject* go) 
{
	RegisterGameObject(go);

	return *this;
}

Hierarchy& Hierarchy::operator-=(GameObject* go) 
{
	QuitGameObject(go);

	return *this;
}


void Hierarchy::RegisterGameObject(GameObject* go)
{
	if (go != nullptr)
	{
		pGameObjectList.push_back(go);

		if (go->transform()->parent() == HIERARCY_ROOT)
			pRootGameObjectList.push_back(go);
	}
}

void Hierarchy::QuitGameObject(GameObject* go)
{
	if (go != nullptr) 
	{
		pGameObjectList.erase(remove(
			pGameObjectList.begin(),
			pGameObjectList.end(), go),
			pGameObjectList.end()
		);

		if (go->transform()->parent() == HIERARCY_ROOT)
			pRootGameObjectList.erase(remove(
				pRootGameObjectList.begin(),
				pRootGameObjectList.end(), go),
				pRootGameObjectList.end()
			);
	}
}


vector<GameObject*> Hierarchy::GetObjectList()
{
	return pGameObjectList;
}

GameObject* Hierarchy::GetObjectByIndex(int index)
{
	if (index < 0 || index >= pGameObjectList.size())
	{
		// DEBUG: out of range
		return nullptr;
	}

	return pGameObjectList[index];
}