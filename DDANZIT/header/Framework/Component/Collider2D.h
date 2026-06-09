#pragma once

#include "Component.h"

#include "Utillity.h"

class Rigidbody2D;
class Collision2D;

enum class ColliderType
{
	Box,
	Circle,
	Capsule,
};



class Collider2D : public Component
{
public:
	friend class DDANZIT_Core;
	friend class GameObject;
	friend class Rigidbody2D;


#pragma region Constructor

protected:
	Collider2D() = default;
	Collider2D(const Collider2D& other);
	Collider2D(GameObject* pGameObject);

public:
	virtual ~Collider2D() = default;

#pragma endregion



#pragma region Clone

private:
	virtual Component* Clone() const override;

#pragma endregion



#pragma region Properties

private:
	bool _isTrigger = true;			// 일단 기본이 활성입니다

public:
	bool& isTrigger() { return _isTrigger; }		// 미뤄야할수도 있으니 일단
	const bool& isTrigger() const { return _isTrigger; }


private:
	Vector2 _offset;
public:
	Vector2& offset() { return _offset; }
	const Vector2& offset() const { return _offset; }

	
private:
	Rigidbody2D* attachedBody;
public:
	const Rigidbody2D attachedRigidBody() const;	// 읽기만 하세요

	// 내부용
protected:
	Vector2 _size;		// 살짝훑기용

	ColliderType type;

#pragma endregion



#pragma region Methods

private:
	bool IsNearby(Vector2 otherOffsetSize);
	bool IsCollideWith(const Collider2D& other);

#pragma endregion

};


// 걍 여따 다써야지

class BoxCollider2D : public Collider2D
{

#pragma region Constructor

public:
	BoxCollider2D() = delete;
	BoxCollider2D(const BoxCollider2D& other);
	BoxCollider2D(GameObject* pGameObject);

public:
	~BoxCollider2D() = default;

#pragma endregion



#pragma region Clone

private:
	Component* Clone() const override;

#pragma endregion



#pragma region Properties

public:
	Vector2& size() { return _size; }
	const Vector2& size() const { return _size; }

#pragma endregion

};


class CircleCollider2D : public Collider2D
{

#pragma region Constructor

public:
	CircleCollider2D() = delete;
	CircleCollider2D(const CircleCollider2D& other);
	CircleCollider2D(GameObject* pGameObject);

public:
	~CircleCollider2D() = default;

#pragma endregion



#pragma region Clone

private:
	Component* Clone() const override;

#pragma endregion



#pragma region Properties

private:
	float _radius;
public:
	float& radius() { return _radius; }
	const float& radius() const { return _radius; }

#pragma endregion

};

// 안할듯
class CapsuleCollider2D : public Collider2D
{

#pragma region Constructor

public:
	CapsuleCollider2D() = delete;
	CapsuleCollider2D(const CapsuleCollider2D& other);
	CapsuleCollider2D(GameObject* pGameObject);

public:
	~CapsuleCollider2D() = default;

#pragma endregion



#pragma region Clone

private:
	Component* Clone() const override;

#pragma endregion



#pragma region Properties

private:
	Vector2 _size;
public:
	Vector2& size() { return _size; }
	const Vector2& size() const { return _size; }

#pragma endregion

};
