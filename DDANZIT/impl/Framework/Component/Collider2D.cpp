#include "Collider2D.h"

#include "DDANZIT_Core.h"
#include "Rigidbody2D.h"
#include "Transform.h"


#pragma region Constructor

Collider2D::Collider2D(GameObject* pGameObject) : Component(pGameObject), _offset(Vector2(0.0f, 0.0f)), attachedBody(nullptr), _size(Vector2(1.0f, 1.0f))
{
	// 생성자에서 넣자
	DDANZIT_Core::collider2DList.push_back(this);
}

Collider2D::Collider2D(const Collider2D& other) : Component(other)
{
	// 강체보다 늦게 복사된 뒤에 그걸 가져와야하네.. 하지마이씨
	// TODO_LATER
}


Collider2D::~Collider2D()
{
	DDANZIT_Core::collider2DList.erase(remove(
		DDANZIT_Core::collider2DList.begin(),
		DDANZIT_Core::collider2DList.end(), this),
		DDANZIT_Core::collider2DList.end());
}

#pragma endregion



#pragma region Clone

Component* Collider2D::Clone() const
{
	return new Collider2D(*this);
}

#pragma endregion


#pragma region Properties

Rigidbody2D* Collider2D::attachedRigidBody() const 
{ 
	return attachedBody; 
}

#pragma endregion



#pragma region Methods

bool Collider2D::IsNearby(Collider2D* other)
{
	// pos scale offset size 까지 해서! 대략적인 크기로 검사
	// 대략적인 크기란 뭘까? ㅎ.. 사각형기준 두배? (최악 90도 회전이면 대강..)
	// -> 걍 대각선 지름의 원을 가정해서 distance 검사하면 된다 와

	Vector2 myOffsetPos = transform()->position() + _offset;
	Vector2 otherOffsetPos = other->transform()->position() + other->_offset;

	Vector2 myScaleSize = Vector2(transform()->scale().x * _size.x, transform()->scale().y * _size.y);
	Vector2 otherScaleSize = Vector2(other->transform()->scale().x * other->_size.x, other->transform()->scale().y * other->_size.y);

	float myRadius = myScaleSize.Length() * 0.5f;			// TODO?: 이걸 캐싱하면 좋다 루트씌워서
	float otherRadius = otherScaleSize.Length() * 0.5f;

	float radiusSum = myRadius + otherRadius;


	if (myOffsetPos.DistanceSquared(otherOffsetPos) <= (radiusSum * radiusSum))
		return true;

	return false;
}

bool Collider2D::IsCollideWith(Collider2D* other)
{
	// 일단 둘다 원 기준 땡처리

	Vector2 myOffsetPos = transform()->position() + _offset;
	Vector2 otherOffsetPos = other->transform()->position() + other->_offset;

	float myRadius = transform()->scale().x * _size.x * 0.5f;
	float otherRadius = other->transform()->scale().x * other->_size.x * 0.5f;

	float radiusSum = myRadius + otherRadius;


	if (myOffsetPos.DistanceSquared(otherOffsetPos) <= (radiusSum * radiusSum))
		return true;

	return false;
}

#pragma endregion