#include "Collider.h"



#pragma region Constructor

Collider::Collider(GameObject* pGameObject) : Component(pGameObject)
{

}

Collider::Collider(const Collider& other) : Component(other)
{

}

#pragma endregion



#pragma region Clone

Component* Collider::Clone() const
{
	return new Collider(*this);
}

#pragma endregion