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

const Rigidbody2D Collider2D::attachedRigidBody() const 
{ 
	return *attachedBody; 
}

#pragma endregion



#pragma region Methods

bool Collider2D::IsNearby(const Collider2D& other)
{
	// TODO
	// 다른쪽 pos scale rot offset size 까지 해서! 대략적인 크기로 검사
	// 대략적인 크기란 뭘까? ㅎ.. 사각형기준 두배? (최악 90도 회전이면 대강..)
	// -> 걍 대각선 지름의 원을 가정해서 distance 검사하면 된다 와

	Vector2 myOffsetPos;
}

bool Collider2D::IsCollideWith(const Collider2D& other)
{

}

#pragma endregion
