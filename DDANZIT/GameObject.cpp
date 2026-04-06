#include "GameObject.h"
#include <vector>

GameObject::GameObject() {
	RegisterObj(this);
}

GameObject::~GameObject() {

}

GameObject* GameObject::Instantiate() {
	// 동적할당~~
	// 자체적으로도 가지고 있는게 낫나?
	GameObject* newInstance = new GameObject;
	return newInstance;
}

void GameObject::Destroy(GameObject* instance) {
	delete(instance);
}

void GameObject::RegisterObj(GameObject* pGameObject) {
	GetObjList().push_back(pGameObject);
}

std::vector<GameObject*> GameObject::GetObjList() {
	static std::vector<GameObject*> objList;
	return objList;
}