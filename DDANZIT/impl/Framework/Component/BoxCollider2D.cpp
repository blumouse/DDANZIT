#include "Collider2D.h"

#include "DDANZIT_Core.h"
#include "Rigidbody2D.h"
#include "Transform.h"


#pragma region Constructor

BoxCollider2D::BoxCollider2D(GameObject* pGameObject) : Collider2D(pGameObject, "BoxCollider2D")
{
	type = ColliderType::Box;
}

BoxCollider2D::BoxCollider2D(const BoxCollider2D& other) : Collider2D(other)
{

}

#pragma endregion



#pragma region Clone

Component* BoxCollider2D::Clone() const
{
	return new BoxCollider2D(*this);
}

#pragma endregion



#pragma region Properties

void BoxCollider2D::SetSize(Vector2 newSize)
{
	_size = newSize;

	SetBoundingRadius();
}

#pragma endregion



#pragma region Methods

bool BoxCollider2D::IsCollideWith(Collider2D* other)
{
	return Collider2D::IsCollideWith(other);


	//if (other->type == ColliderType::Box)
	//{
	//	// 회전값이 혹시 같으면..? AABB로 땡처리 가능하다!
	//	Vector2 myDir = transform()->direction();
	//	Vector2 otherDir = other->transform()->direction();

	//	float dot = myDir.Dot(otherDir);

	//	if (abs(dot) > 0.99f)	// 널널하게
	//	{

	//	}

	//	// 아니면 뭐..
	//}
	//else if (other->type == ColliderType::Circle)
	//{
	//	// 으엑
	//}
	//else
	//{
	//	// DEBUG: 미정의
	//	return false;
	//}


	//return false;
}

#pragma endregion
