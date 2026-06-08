#include "Draw2D.h"

#include "DDANZIT_Core.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"

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
				transform->direction().x,
				transform->direction().y,
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
		// TODO: 액티브 검사

		// 초록
		int colorRGBA = 0x00ff00ff;

		DDANZIT_Core::debugDrawCommandLists[layer].push_back(
			DebugDrawCommand{
				transform->position().x,
				transform->position().y,
				transform->scale().x,
				transform->scale().y,
				// TODO: 타입
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