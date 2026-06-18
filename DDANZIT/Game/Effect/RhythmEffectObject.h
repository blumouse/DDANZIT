#pragma once

#include "Draw2D.h"
#include "GameObject.h"

// 씬에 미리 올려두는 이펙트 오브젝트입니다.
// 경고등/파편/공격 표시가 모두 같은 SpriteRenderer 기반 풀을 공유합니다.
class RhythmEffectObject : public GameObject, public Draw2D
{
public:
	RhythmEffectObject(Scene* scene);
	RhythmEffectObject(const RhythmEffectObject& other) = default;

	GameObject* Clone() const override;
};
