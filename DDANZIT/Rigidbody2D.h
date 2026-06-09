#pragma once

#include "Component.h"


class Rigidbody2D : public Component
{
public:
	friend class DDANZIT_Core;
	friend class GameObject;
	friend class Collider2D;


#pragma region Constructor

protected:
	Rigidbody2D() = default;
	Rigidbody2D(const Rigidbody2D& other);
	Rigidbody2D(GameObject* pGameObject);

public:
	virtual ~Rigidbody2D() = default;

#pragma endregion



#pragma region Clone

private:
	virtual Component* Clone() const override;

};

