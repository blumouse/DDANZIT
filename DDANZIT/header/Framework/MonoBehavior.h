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

private:
	Component* Clone() const override;

#pragma endregion



#pragma region Properties

private:
	bool isInUpdateList;

#pragma endregion



#pragma region Methods

public:
	void SetActive(bool newActive) override;


	// 내부용
private:
	void SetParentActive(bool newActive) override;

#pragma endregion


};

