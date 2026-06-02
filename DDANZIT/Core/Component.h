#pragma once


class Scene;
class GameObject;
class MonoBehavior;

class Component
{
public:
	friend class GameObject;
	friend class MonoBehavior;

#pragma region Constructor

protected:
	Component() = delete;
	Component(const Component&) = delete;
	Component(GameObject* pGameObject);

public:
	virtual ~Component() = default;

#pragma endregion



#pragma region Properites

private:
	GameObject* _gameObject;
	// 강력하게 세터는 안놔두기
public:
	GameObject* const gameObject();				// 포인터의 경우.. 주소만 잠그고 내부는 알아서


protected:
	bool _active;
	//bool& active() { return _active; }
public:
	const bool& active() const { return _active; }


	// 내부용
private:
	// 내 오브젝트가 어떤지, 둘 중 하나라도 false면 꺼진거임 상태 저장용이다
	bool parentActive = true;

#pragma endregion



#pragma region Methods

public:
	// 매우매우 고민하다 상속주기..
	virtual void SetActive(bool newActive);


	// 내부용
private:
	virtual void SetParentActive(bool newActive);

#pragma endregion



#pragma region Component

public:
	template <std::derived_from<Component> T>
	void GetComponents(std::vector<T*>& componentList) const
	{
		componentList.clear();

		for (Component* comp : _gameObject->pComponentList)
		{
			if (T* targetComponent = dynamic_cast<T*>(comp))
				componentList.push_back(targetComponent);
		}
	}

	template <std::derived_from<Component> T>
	T* GetComponent() const
	{
		for (Component* comp : _gameObject->pComponentList)
		{
			if (T* targetComponent = dynamic_cast<T*>(comp))
				return targetComponent;
		}

		return nullptr;
	}

	template <std::derived_from<Component> T>
	bool TryGetComponent(T*& component) const
	{
		for (Component* comp : _gameObject->pComponentList)
		{
			if (T* targetComponent = dynamic_cast<T*>(comp))
			{
				component = targetComponent;
				return true;
			}
		}

		return false;
	}

#pragma endregion

};