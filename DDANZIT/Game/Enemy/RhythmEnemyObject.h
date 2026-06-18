#pragma once

#include "Draw2D.h"
#include "GameObject.h"

// 실제로 처치할 적 오브젝트입니다.
// 모든 적은 Main.cpp에서 미리 풀로 등록되고, 게임 중에는 켜고 끄며 재사용합니다.
class RhythmEnemyObject : public GameObject, public Draw2D
{
public:
	RhythmEnemyObject(Scene* scene);
	RhythmEnemyObject(const RhythmEnemyObject& other) = default;

	GameObject* Clone() const override;
};
