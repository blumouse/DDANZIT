#pragma once

#include <concepts>
#include <string>
#include <vector>

#include "DefineOption.h"

class GameObject;
class Transform;


class Component
{
public:
	friend class DDANZIT_Core;
	friend class GameObject;
	friend class Transform;
	friend class MonoBehavior;
	friend class Camera;

#pragma region Constructor

protected:
	Component() = delete;
	Component(const Component& other) = default;
	Component(GameObject* pGameObject);
	Component(GameObject* pGameObject, bool active);

public:
	virtual ~Component() = default;

#pragma endregion



#pragma region Clone

private:
	virtual Component* Clone() const = 0;

#pragma endregion



#pragma region Properites

private:
	GameObject* _gameObject;
	// 강력하게 세터는 안놔두기
public:
	GameObject* const gameObject();				// 포인터의 경우.. 주소만 잠그고 내부는 알아서


public:
	const std::string& name() const;


public:
	const Tag& tag() const;


public:
	Transform* const transform();


	// 내부용
private:
	bool _active;
	// 내 오브젝트가 어떤지, 둘 중 하나라도 false면 꺼진거임 상태 저장용이다
	bool parentActive;

	// Destroy 마킹
	// TODO_LATER: 컴포넌트 파괴 대응.. 인데 이거 오브젝트 상속받는게 전제니까 플래그가 거기로 올라가겠네?
	bool isKilled = false;

#pragma endregion



#pragma region Methods

public:
	// 매우매우 고민하다 상속주기..
	virtual void SetActive(bool newActive);
	bool isActiveAndEnabled() const { return _active && parentActive; }


	// 내부용
private:
	virtual void SetParentActive(bool newActive);

#pragma endregion



#pragma region Component

public:
	template <std::derived_from<Component> T>
	void GetComponents(std::vector<T*>& componentList) const;

	template <std::derived_from<Component> T>
	T* GetComponent() const;

	template <std::derived_from<Component> T>
	bool TryGetComponent(T*& component) const;

#pragma endregion

};