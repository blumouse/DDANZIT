#pragma once

#include "Draw2D.h"
#include "GameObject.h"

// Text만 가진 Draw2D는 기존 렌더러가 보조 SpriteRenderer를 참조할 수 있습니다.
// 그래서 이 래퍼는 비활성 SpriteRenderer와 Text를 함께 붙여 안전하게 씁니다.
class UiTextObject : public GameObject, public Draw2D
{
public:
	UiTextObject(Scene* scene);
	UiTextObject(const UiTextObject& other) = default;

	GameObject* Clone() const override;
};
