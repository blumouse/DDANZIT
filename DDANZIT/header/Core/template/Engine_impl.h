#pragma once

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

	// DEBUG: 그런 컴포넌트 없음 메세지
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
		// DEBUG: 디버그 메세지
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
		// DEBUG: 디버그 메세지
		return nullptr;
	}

	T* go = new T(scene);		// 여기서 유저설정한 컴포넌트들 일단 붙어서 나옴


	pGameObjectList.push_back(go);
	go->ownerHierarchy = this;

	if (go->transform()->parent() == HIERARCY_ROOT)
		pRootGameObjectList.push_back(go);

	for (Component* comp : go->pComponentList)
	{
		if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(comp))
		{
			go->InitializeLifecycle(b);
		}
	}

	go->isInitialized = true;

	return go;
}

#pragma endregion
