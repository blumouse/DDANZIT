#include "Collider2D.h"

#include "DDANZIT_Core.h"
#include "Rigidbody2D.h"
#include "Transform.h"


#pragma region Constructor

CircleCollider2D::CircleCollider2D(GameObject* pGameObject) : Collider2D(pGameObject, "CircleCollider2D")
{
	type = ColliderType::Circle;
	_size.y = 0.0f;
}

CircleCollider2D::CircleCollider2D(const CircleCollider2D& other) : Collider2D(other)
{

}

#pragma endregion



#pragma region Clone

Component* CircleCollider2D::Clone() const
{
	return new CircleCollider2D(*this);
}

#pragma endregion



#pragma region Clone

void CircleCollider2D::SetRadius(float newRadius)
{
	_size.x = newRadius;

	SetBoundingRadius();
}

#pragma endregion



#pragma region Methods

bool CircleCollider2D::IsCollideWith(Collider2D* other)
{
	return Collider2D::IsCollideWith(other);


	//if (other->type == ColliderType::Circle)
	//{
	//	Vector2 myOffsetPos = transform()->position() + _offset;
	//	Vector2 otherOffsetPos = other->transform()->position() + other->_offset;

	//	float myRadius = transform()->scale().x * _size.x * 0.5f;
	//	float otherRadius = other->transform()->scale().x * other->_size.x * 0.5f;

	//	float radiusSum = myRadius + otherRadius;


	//	if (myOffsetPos.DistanceSquared(otherOffsetPos) <= (radiusSum * radiusSum))
	//		return true;

	//	return false;
	//}
	//else if (other->type == ColliderType::Box)
	//{
	//	// ¿∏ø¢
	//}
	//else
	//{
	//	// DEBUG: πÃ¡§¿«
	//	return false;
	//}

	//return false;
}

#pragma endregion
