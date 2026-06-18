#pragma once

#include "MonoBehavior.h"
#include "Utillity.h"
#include "Game/Common/RhythmTypes.h"

#include <vector>

class SpriteRenderer;
class Collider2D;
class RhythmGameManager;
class PlayerActionHitboxScript;

//수정완 프로젝트의 EnemyState를 컴포넌트로 분리한 클래스입니다.
// 한 슬롯은 비활성 상태로 대기하다가 Activate에서 방향, 이동 경로, 판정 시간을 새로 받습니다.
class RhythmEnemyController : public MonoBehavior
{
public:
	RhythmEnemyController(GameObject* gameObject);

	void Awake() override;
	void Update() override;
	void OnTriggerEnter2D(Collider2D* collision) override;
	void OnTriggerStay2D(Collider2D* collision) override;

	void InitializePoolSlot(int slotIndex);
	void Activate(
		RhythmDirection direction,
		Vector2 startPosition,
		Vector2 targetPosition,
		double spawnTime,
		double hitTime,
		float hitWindowSeconds,
		float secondsPerBeat,
		float movePortion,
		float movePower,
		int travelBeats,
		RhythmGameManager* manager);
	void Deactivate();
	void Kill();
	void Miss();
	void UpdateFromSongTime(double songTime);

	bool IsActiveEnemy() const;
	bool IsVulnerableEnemy() const;
	bool CanBeHit(RhythmDirection inputDirection, double songTime) const;
	bool HasMissed(double songTime) const;
	bool TryConsumeApproachLight(double songTime, Vector2& lightPosition, bool& fastLight);
	double HitErrorSeconds(double songTime) const;
	RhythmDirection Direction() const;
	Vector2 Position();
	bool TryResolvePlayerAction(PlayerActionHitboxScript* hitbox);

private:
	void ApplyVisual();
	void ApplyWalkFrame(int frameIndex);
	void ApplyParryVisual();
	void UpdateParryAnimationFrame();
	void PrepareDeathMotion();
	bool TryResolvePlayerAction(Collider2D* collision);

	RhythmGameManager* manager = nullptr;
	int slotIndex = -1;
	bool activeEnemy = false;
	bool dyingEnemy = false;
	bool missedEnemy = false;
	bool approachLightShown = false;
	RhythmDirection direction = RhythmDirection::Left;
	Vector2 startPosition = Vector2(0.0f, 0.0f);
	Vector2 targetPosition = Vector2(0.0f, 0.0f);
	std::vector<Vector2> stepPoints;
	double spawnTime = 0.0;
	double hitTime = 0.0;
	float hitWindowSeconds = 0.22f;
	float secondsPerBeat = 0.5f;
	float movePortion = 0.22f;
	float deathTimerSeconds = 0.0f;
	Vector2 deathVelocity = Vector2(0.0f, 0.0f);
	float missTimerSeconds = 0.0f;
	float missDurationSeconds = 0.0f;
	int travelBeats = 3;
	int currentWalkFrame = -1;
	SpriteRenderer* spriteRenderer = nullptr;
};
