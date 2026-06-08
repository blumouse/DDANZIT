#pragma once

#include "Component.h"


// TODO: 이거 상속주기
class Collider : public Component
{

#pragma region Constructor

protected:
	Collider() = default;
	Collider(const Collider& other);
	Collider(GameObject* pGameObject);

public:
	virtual ~Collider() = default;

#pragma endregion



#pragma region Clone

private:
	Component* Clone() const override;

#pragma endregion

};

