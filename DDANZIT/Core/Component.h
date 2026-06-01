#pragma once


class GameObject;

class Component
{

#pragma region Constructor

public:
	Component() = delete;
	Component(const Component&) = delete;
	Component(GameObject* pGameObject);

	virtual ~Component() = default;

#pragma endregion



#pragma region Properites

private:
	GameObject* _gameObject;
	// 강력하게 세터는 안놔두기
public:
	GameObject* const gameObject();				// 포인터의 경우.. 주소만 잠그고 내부는 알아서

#pragma endregion



#pragma region LifeCycles

	virtual void Awake() {}
	virtual void OnEnable() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void FixedUpdate() {}

	virtual void OnDisable() {}
	virtual void OnDestroy() {}

#pragma endregion

};