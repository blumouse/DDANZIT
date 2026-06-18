#pragma once

#include "MonoBehavior.h"
#include "Utillity.h"
#include "Game/Common/RhythmTypes.h"

class SpriteRenderer;
class Collider2D;
class RhythmGameManager;
class PlayerActionHitboxScript;

// 수정완 프로젝트의 AttackState를 컴포넌트화한 가드 공격 슬롯입니다.
// 경고등이 먼저 뜬 뒤 launchTime부터 실제 오브젝트가 켜지고, arrivalTime 근처에서 방어할 수 있습니다.
class RhythmGuardAttackController : public MonoBehavior
{
public:
	RhythmGuardAttackController(GameObject* gameObject);

	void Awake() override;
	void Update() override;
	void OnTriggerEnter2D(Collider2D* collision) override;
	void OnTriggerStay2D(Collider2D* collision) override;

	void InitializePoolSlot(int slotIndex);
	void Activate(
		RhythmDirection direction,
		Vector2 startPosition,
		Vector2 targetPosition,
		double launchTime,
		double hitTime,
		float hitWindowSeconds,
		RhythmGameManager* manager);
	void Deactivate();
	void Block();
	void Miss();
	void UpdateFromSongTime(double songTime);

	bool IsActiveAttack() const;
	bool CanBeBlocked(RhythmDirection inputDirection, double songTime) const;
	bool HasMissed(double songTime) const;
	double HitErrorSeconds(double songTime) const;
	RhythmDirection Direction() const;
	Vector2 Position();
	bool TryResolvePlayerAction(PlayerActionHitboxScript* hitbox);

private:
	void ApplyVisual();
	void UpdateAnimationFrame(double songTime);
	void ApplyParryVisual();
	void UpdateParryAnimationFrame();
	void PrepareBlockMotion();
	bool TryResolvePlayerAction(Collider2D* collision);

	RhythmGameManager* manager = nullptr;
	int slotIndex = -1;
	bool activeAttack = false;
	bool blockedAttack = false;
	bool visible = false;
	RhythmDirection direction = RhythmDirection::Left;
	Vector2 startPosition = Vector2(0.0f, 0.0f);
	Vector2 targetPosition = Vector2(0.0f, 0.0f);
	Vector2 blockVelocity = Vector2(0.0f, 0.0f);
	double launchTime = 0.0;
	double hitTime = 0.0;
	float hitWindowSeconds = 0.22f;
	float blockTimerSeconds = 0.0f;
	float blockDurationSeconds = 1.0f;
	SpriteRenderer* spriteRenderer = nullptr;
};
