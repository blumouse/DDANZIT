#pragma once

#include "Component.h"
#include "Lifecycle.h"

class GameObject;


// 하지만 Mono가 아닌
// 스크립트에 다세요
class MonoBehavior : public Component, public Lifecycle
{
public:
	friend class DDANZIT_Core;
	friend class Scene;
	friend class GameObject;

#pragma region Constructor

protected:
	MonoBehavior() = default;
	MonoBehavior(const MonoBehavior& other);
	MonoBehavior(GameObject* pGameObject);

public:
	virtual ~MonoBehavior() = default;

#pragma endregion



#pragma region Clone

	// 트랜스폼은 고정이라 전용으로 하면 될거같은데? 가려버려
private:
	Component* Clone() const override;

#pragma endregion



#pragma region Properties

private:
	bool isInUpdateList;

#pragma endregion



#pragma region Methods

public:
	const bool& isActiveAndEnabled() const { return _active && parentActive; }
	void SetActive(bool newActive) override;


	// 내부용
private:
	void SetParentActive(bool newActive) override;

#pragma endregion


};

