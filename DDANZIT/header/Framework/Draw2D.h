#pragma once

#include <vector>

#include "DefineOption.h"
#include "IDrawable.h"

class GameObject;
class SpriteRenderer;

#ifdef USE_DEBUG

class Collider2D;
//class CircleCollider2D;
//class BoxCollider2D;
//class CapsuleCollider2D;

#endif // USE_DEBUG



#ifdef PROPS_MODE_2D

// 일반 게임오브젝트용
class Draw2D : public IDrawable
{
public:
	friend class GameObject;
	friend class Transform;

#pragma region Constructor

protected:
	Draw2D() = delete;
	Draw2D(const Draw2D&) = default;
	Draw2D(GameObject* pGameObject);

public:
	virtual ~Draw2D();

#pragma endregion



#pragma region Properties

private:
	int layer = 0;
	GameObject* gameObject;

	SpriteRenderer* spriteRenderer;

#ifdef USE_DEBUG

	std::vector<Collider2D*> pColliderList;

#endif // USE_DEBUG

#pragma endregion



#pragma region IDrawable

private:
	void Draw() override;

	// depth랑 연동해서 로직타임에 바꿔놓기로 하자
	int GetLayer() override;
	void SetLayer(int layer) override;

#pragma endregion

};

#endif // PROPS_MODE_2D

