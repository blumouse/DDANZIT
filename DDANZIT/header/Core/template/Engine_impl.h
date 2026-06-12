#pragma once

#include "Debug.h"

// 탬플릿 구현부


#pragma region GameObject


template <std::derived_from<Component> T>
void GameObject::GetComponents(std::vector<T*>& componentList) const
{
	componentList.clear();

	for (Component* comp : pComponentList)
	{
		if (comp->isKilled)
			continue;

		if (T* targetComponent = dynamic_cast<T*>(comp))
			componentList.push_back(targetComponent);
	}
}

template <std::derived_from<Component> T>
T* GameObject::GetComponent() const
{
	for (Component* comp : pComponentList)
	{
		if (comp->isKilled)
			continue;

		if (T* targetComponent = dynamic_cast<T*>(comp))
			return targetComponent;
	}

	Debug::Log("GetComponent: 컴포넌트를 찾지 못했습니다.");
	return nullptr;
}

template <std::derived_from<Component> T>
bool GameObject::TryGetComponent(T*& component) const
{
	for (Component* comp : pComponentList)
	{
		if (comp->isKilled)
			continue;

		if (T* targetComponent = dynamic_cast<T*>(comp))
		{
			component = targetComponent;
			return true;
		}
	}

	return false;
}


template <std::derived_from<Component> T>
T* GameObject::AddComponent()
{
	if (pComponentList.size() == MAX_COMPONENT_NUM)
	{
		Debug::Log("AddComponent: 컴포넌트 수가 최대입니다.");
		return nullptr;
	}

	T* component = new T(this);

	pComponentList.push_back(component);


	if (isInitialized)
	{
		if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(component))
		{
			InitializeLifecycle(b);
		}
	}

	return component;
}


// 기본 컴포넌트들은 선언만

template <>
SpriteRenderer* GameObject::AddComponent<SpriteRenderer>();

template <>
Camera* GameObject::AddComponent<Camera>();

template <>
BoxCollider2D* GameObject::AddComponent<BoxCollider2D>();

template <>
CircleCollider2D* GameObject::AddComponent<CircleCollider2D>();

template <>
Rigidbody2D* GameObject::AddComponent<Rigidbody2D>();

template <>
Text* GameObject::AddComponent<Text>();

#pragma endregion



#pragma region Component

template <std::derived_from<Component> T>
void Component::GetComponents(std::vector<T*>& componentList) const
{
	componentList.clear();

	for (Component* comp : _gameObject->pComponentList)
	{
		if (comp->isKilled)
			continue;

		if (T* targetComponent = dynamic_cast<T*>(comp))
			componentList.push_back(targetComponent);
	}
}

template <std::derived_from<Component> T>
T* Component::GetComponent() const
{
	for (Component* comp : _gameObject->pComponentList)
	{
		if (comp->isKilled)
			continue;

		if (T* targetComponent = dynamic_cast<T*>(comp))
			return targetComponent;
	}

	Debug::Log("GetComponent: 컴포넌트를 찾지 못했습니다.");
	return nullptr;
}

template <std::derived_from<Component> T>
bool Component::TryGetComponent(T*& component) const
{
	for (Component* comp : _gameObject->pComponentList)
	{
		if (T* targetComponent = dynamic_cast<T*>(comp))
		{
			if (comp->isKilled)
				continue;

			component = targetComponent;
			return true;
		}
	}

	return false;
}

#pragma endregion


#pragma region Hierarchy

template <std::derived_from<GameObject> T>
T* Hierarchy::AddGameObject()
{
	if (pGameObjectList.size() == MAX_SCENE_GAME_OBJECT_NUM)
	{
		Debug::Log("AddGameObject: 오브젝트 수가 최대입니다.");
		return nullptr;
	}

	T* go = new T(scene);		// 여기서 유저설정한 컴포넌트들 일단 붙어서 나옴


	for (Component* comp : go->pComponentList)
	{
		if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(comp))
		{
			go->InitializeLifecycle(b);
		}
	}

	go->isInitialized = true;


	*this += go;

	return go;
}

template <std::derived_from<GameObject> T>
T* Hierarchy::AddGameObject(const std::string& name)
{
	if (pGameObjectList.size() == MAX_SCENE_GAME_OBJECT_NUM)
	{
		Debug::Log("AddGameObject: 오브젝트 수가 최대입니다.");
		return nullptr;
	}

	T* go = new T(scene);
	go->_name = name;


	for (Component* comp : go->pComponentList)
	{
		if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(comp))
		{
			go->InitializeLifecycle(b);
		}
	}

	go->isInitialized = true;


	*this += go;

	return go;
}

#pragma endregion
