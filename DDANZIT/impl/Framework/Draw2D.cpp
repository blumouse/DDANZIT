#include "Draw2D.h"

#include "DDANZIT_Core.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"

#ifdef USE_DEBUG
#include "Collider2D.h"

#endif // USE_DEBUG

using namespace std;


#ifdef PROPS_MODE_2D


#pragma region Constructor

Draw2D::Draw2D(GameObject* pGameObject) : gameObject(pGameObject), spriteRenderer(nullptr)
{
	DDANZIT_Core::RegisterDrawable(this);
}


Draw2D::~Draw2D() 
{
	DDANZIT_Core::QuitDrawable(this);
}

#pragma endregion



#pragma region IDrawable

void Draw2D::Draw() 
{
	// TODO: 그리는 컴포넌트들 AddComponent 탬플릿 명시적 인스턴스 및 구현
	// 트랜스폼에서 뎁스 바뀌면 바로 이거 바꿔주기

	// TODO: 콜라이더 만든후 디버그 드로우

	if (!gameObject->_active || !gameObject->parentActive || gameObject->isKilled)
		return;


	Transform* transform = gameObject->_transform;

	if (spriteRenderer && spriteRenderer->isActiveAndEnabled())
	{
		const Color& c = spriteRenderer->color;

		int colorRGBA =
			((int)(c.r * 255.0f) << 24) |
			((int)(c.g * 255.0f) << 16) |
			((int)(c.b * 255.0f) << 8) |
			((int)(c.a * 255.0f));

		DDANZIT_Core::drawCommandLists[layer].push_back(
			DrawCommand{
				transform->position().x,
				transform->position().y,
				transform->scale().x,
				transform->scale().y,
				transform->angle(),
				spriteRenderer->sprite,
				colorRGBA,
				spriteRenderer->flipX,
				spriteRenderer->flipY,
				false,
				0,
				0,
				0,
				0
			});
	}

	// TODO: 아틀라스를 만들어야하나 유니티에 어케돼있지?

#ifdef USE_DEBUG

	for (Collider2D* col : pColliderList)
	{
		if (!col->isActiveAndEnabled())
			continue;


		int colorRGBA = 0x44ff44ff;		// 초록
		DebugDrawType type;

		switch (col->type)
		{
		default:
			continue;

		case ColliderType::Box:
			type = DebugDrawType::BoxCollider;	break;

		case ColliderType::Circle:
			type = DebugDrawType::CircleCollider;	break;

		case ColliderType::Capsule:
			type = DebugDrawType::CapsuleCollider;	break;
		}


		DDANZIT_Core::debugDrawCommandLists[layer].push_back(
			DebugDrawCommand{
				transform->position().x + col->_offset.x,
				transform->position().y + col->_offset.y,
				transform->scale().x * col->_size.x,
				transform->scale().y * col->_size.y,
				transform->angle(),
				type,
				colorRGBA
			});
	}

#endif // USE_DEBUG

}


int Draw2D::GetLayer() 
{
	return layer;
}

void Draw2D::SetLayer(int layer) 
{
	this->layer = layer;
}

#pragma endregion


#endif // PROPS_MODE_2D