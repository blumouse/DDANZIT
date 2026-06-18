#pragma once

#include "Draw2D.h"
#include "GameObject.h"

// 체력바는 씬에 미리 배치되는 Draw2D 오브젝트입니다.
// SpriteRenderer는 실제 막대를 그리고, HealthBarObserver가 PlayerController의 체력 변경 콜백을 따라갑니다.
class HealthBarObject : public GameObject, public Draw2D
{
public:
	HealthBarObject(Scene* scene);
	HealthBarObject(const HealthBarObject& other) = default;

	GameObject* Clone() const override;
};
