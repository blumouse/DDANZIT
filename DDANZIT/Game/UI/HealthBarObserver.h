#pragma once

#include "MonoBehavior.h"
#include "Utillity.h"

class PlayerController;
class SpriteRenderer;

// HealthBarObserver는 PlayerController의 체력 변경 콜백을 구독해 막대의 길이와 색을 갱신합니다.
// 매 프레임 Update로 폴링하지 않고, 체력이 바뀐 순간에만 표시를 바꾸도록 분리했습니다.
class HealthBarObserver : public MonoBehavior
{
public:
	HealthBarObserver(GameObject* gameObject);

	void Awake() override;

	void SetTarget(PlayerController* targetPlayer);
	void ResetView();

private:
	static void OnHealthChanged(void* context, int currentHealth, int maxHealth);
	void ApplyHealth(int currentHealth, int maxHealth);

	PlayerController* targetPlayer = nullptr;
	SpriteRenderer* spriteRenderer = nullptr;
	Vector2 baseScale = Vector2(1.0f, 1.0f);
	bool hasBaseScale = false;
	int cachedCurrentHealth = 0;
	int cachedMaxHealth = 1;
};
