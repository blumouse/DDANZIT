#include "Rigidbody2D.h"

#include "DDANZIT_Core.h"
#include "Collider2D.h"

using namespace std;


#pragma region Constructor

Rigidbody2D::Rigidbody2D(GameObject* pGameObject) : Component(pGameObject)
{
}

//Rigidbody2D::Rigidbody2D(const Rigidbody2D& other) : Component(other)
//{
//
//}


Rigidbody2D::~Rigidbody2D()
{
}

#pragma endregion



#pragma region Clone

Component* Rigidbody2D::Clone() const
{
	return new Rigidbody2D(*this);
}

#pragma endregion



#pragma region Properties

vector<Collider2D*> Rigidbody2D::GetAttachedColliders() const
{
	return attachedColliderList;
}


#pragma endregion
