#pragma once

#include "Component.h"
#include "Lifecycle.h"

class GameObject;


// 하지만 Mono가 아닌
// 스크립트에 다세요
class MonoBehavior : public Component, public Lifecycle
{
public:
	friend class GameObject;

#pragma region Constructor

protected:
	MonoBehavior() = default;
	MonoBehavior(const MonoBehavior&) = delete;
	MonoBehavior(GameObject* pGameObject);

public:
	virtual ~MonoBehavior() = default;

#pragma endregion



#pragma region Methods

public:
	// 매우매우 고민하다 상속주기..
	void SetActive(bool newActive) override;


	// 내부용
private:
	void SetParentActive(bool newActive) override;

#pragma endregion


};

