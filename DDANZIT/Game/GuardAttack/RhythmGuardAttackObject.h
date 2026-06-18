#pragma once

#include "Draw2D.h"
#include "GameObject.h"

// 씬에 미리 올려두는 가드 공격 오브젝트입니다.
// SpriteRenderer와 CircleCollider2D를 가진 뒤 RhythmGuardAttackController가 활성/비활성을 관리합니다.
class RhythmGuardAttackObject : public GameObject, public Draw2D
{
public:
	RhythmGuardAttackObject(Scene* scene);
	RhythmGuardAttackObject(const RhythmGuardAttackObject& other) = default;

	GameObject* Clone() const override;
};
