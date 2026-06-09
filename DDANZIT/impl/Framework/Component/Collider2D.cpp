#include "Collider2D.h"

#include "Rigidbody2D.h"


#pragma region Constructor

Collider2D::Collider2D(GameObject* pGameObject) : Component(pGameObject), _offset(Vector2(0.0f, 0.0f)), attachedBody(nullptr), size(Vector2(1.0f, 1.0f))
{

}

Collider2D::Collider2D(const Collider2D& other) : Component(other)
{

}

#pragma endregion



#pragma region Clone

Component* Collider2D::Clone() const
{
	return new Collider2D(*this);
}

#pragma endregion


#pragma region Properties

const Rigidbody2D Collider2D::attachedRigidBody() const 
{ 
	return *attachedBody; 
}

#pragma endregion



#pragma region Methods

bool Collider2D::IsNearby(Vector2 otherOffsetSize)
{
	// 코어에서 다른쪽 pos scale rot offset size 까지 해서! 대략적인 크기로 넘겨준다
}

bool Collider2D::IsCollideWith(const Collider2D& other)
{

}

#pragma endregion
