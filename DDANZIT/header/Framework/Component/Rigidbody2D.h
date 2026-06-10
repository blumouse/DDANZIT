#pragma once

#include <vector>

#include "Component.h"

enum class RigidBodyType2D
{
	Dynamic,
	Kinematic,
	Static,
};


class Rigidbody2D : public Component
{
public:
	friend class DDANZIT_Core;
	friend class GameObject;
	friend class Collider2D;


#pragma region Constructor

public:
	Rigidbody2D() = delete;
	Rigidbody2D(const Rigidbody2D& other) = default;
	Rigidbody2D(GameObject* pGameObject);

public:
	~Rigidbody2D() = default;

#pragma endregion



#pragma region Clone

private:
	Component* Clone() const override;

#pragma endregion



#pragma region Properties

public:
	RigidBodyType2D bodyType = RigidBodyType2D::Kinematic;		// 을 디폴트로

private:
	std::vector<Collider2D*> attachedColliderList;
public:
	std::vector<Collider2D*> GetAttachedColliders() const;


private:
	std::vector<Collider2D*> prevCollideList;

#pragma endregion

};

