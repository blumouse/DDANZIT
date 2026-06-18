#pragma once

#include "Color.h"
#include "DefineOption.h"
#include "MonoBehavior.h"
#include "Utillity.h"

class SpriteRenderer;

enum class RhythmEffectType
{
	Light,
	Shard
};

// 짧은 수명의 경고등/파편 효과를 담당하는 풀 슬롯 컴포넌트입니다.
// GameObject를 새로 만들지 않고 Activate/Deactivate로 재사용합니다.
class RhythmEffectController : public MonoBehavior
{
public:
	RhythmEffectController(GameObject* gameObject);
	void Awake() override;

	void InitializePoolSlot(int slotIndex);
	void Activate(
		RhythmEffectType type,
		SpriteIndex sprite,
		Vector2 position,
		Vector2 scale,
		Color color,
		float lifeSeconds,
		Vector2 velocity = Vector2(0.0f, 0.0f));
	void Deactivate();
	void Tick(float deltaSeconds);

	bool IsActiveEffect() const;

private:
	int slotIndex = -1;
	bool activeEffect = false;
	RhythmEffectType type = RhythmEffectType::Shard;
	Vector2 velocity = Vector2(0.0f, 0.0f);
	Vector2 baseScale = Vector2(1.0f, 1.0f);
	Color baseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	float ageSeconds = 0.0f;
	float lifeSeconds = 0.1f;
	SpriteRenderer* spriteRenderer = nullptr;
};
