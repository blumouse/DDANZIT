#include "Game/GuardAttack/RhythmGuardAttackController.h"

#include "DDANZITEngine.h"
#include "Game/Manager/RhythmGameManager.h"
#include "Game/Player/PlayerActionHitboxScript.h"

#include <algorithm>
#include <cmath>

namespace
{
	constexpr int kAttackFrameColumns = 8;
	constexpr int kAttackFrameRows = 3;
	constexpr int kAttackFrameCount = kAttackFrameColumns * kAttackFrameRows;
	constexpr int kAttackFrameWidth = 192;
	constexpr int kAttackFrameHeight = 341;
	constexpr double kAttackFrameRate = 24.0;
	constexpr float kAttackSpriteScale = 3.f;
	constexpr int kParryFrameColumns = 8;
	constexpr int kParryFrameRows = 3;
	constexpr int kParryFrameCount = kParryFrameColumns * kParryFrameRows;
	constexpr int kParryFrameSize = 256;
	constexpr float kParrySpriteScale = 700.f;

	Vector2 LerpVector(const Vector2& from, const Vector2& to, float t)
	{
		return from + (to - from) * t;
	}

	float GuardAttackAngleForDirection(RhythmDirection direction)
	{
		const Vector2 travelDirection = RhythmDirectionVector(direction) * -1.0f;
		return std::atan2(travelDirection.y, travelDirection.x) * Transform::RAD2DEG * 7;
	}
}

RhythmGuardAttackController::RhythmGuardAttackController(GameObject* gameObject) : MonoBehavior(gameObject)
{
	EnableAwake();
	EnableUpdate();
	EnableOnTriggerEnter2D();
	EnableOnTriggerStay2D();
}

void RhythmGuardAttackController::Awake()
{
	spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();
	Deactivate();
}

void RhythmGuardAttackController::InitializePoolSlot(int slotIndex)
{
	this->slotIndex = slotIndex;
	Deactivate();
}

void RhythmGuardAttackController::Activate(
	RhythmDirection direction,
	Vector2 startPosition,
	Vector2 targetPosition,
	double launchTime,
	double hitTime,
	float hitWindowSeconds,
	RhythmGameManager* manager)
{
	this->manager = manager;
	this->direction = direction;
	this->startPosition = startPosition;
	this->targetPosition = targetPosition;
	this->launchTime = launchTime;
	this->hitTime = hitTime;
	this->hitWindowSeconds = hitWindowSeconds;
	activeAttack = true;
	blockedAttack = false;
	visible = false;
	blockTimerSeconds = 0.0f;
	blockVelocity = Vector2(0.0f, 0.0f);

	transform()->SetPosition(startPosition);
	transform()->SetScale(Vector2(kAttackSpriteScale, kAttackSpriteScale));
	transform()->SetLocalAngle(GuardAttackAngleForDirection(direction));
	ApplyVisual();
	//컴포넌트 Update가 launchTime에 스스로 켜야 하므로 오브젝트는 활성 상태로 둡니다.
	// launchTime 전에는 renderer alpha만 0으로 숨기고, 실제 비활성화는 Block/Miss/Deactivate에서만 합니다.
	if (spriteRenderer != nullptr)
		spriteRenderer->color = Color(1.0f, 1.0f, 1.0f, 0.0f);
	gameObject()->SetActive(true);
}

void RhythmGuardAttackController::Update()
{
	if (blockedAttack)
	{
		const float deltaSeconds = std::clamp(Time::deltaTime(), 0.0f, 0.05f);
		transform()->SetPosition(transform()->position() + blockVelocity * deltaSeconds);
		blockVelocity = LerpVector(blockVelocity, Vector2(0.0f, 0.0f), std::clamp(deltaSeconds * 7.5f, 0.0f, 1.0f));
		blockTimerSeconds -= deltaSeconds;
		UpdateParryAnimationFrame();

		if (spriteRenderer != nullptr)
			spriteRenderer->color = Color(1.0f, 1.0f, 1.0f, std::clamp(blockTimerSeconds / blockDurationSeconds, 0.0f, 0.95f));

		if (blockTimerSeconds <= 0.0f)
			Deactivate();
		return;
	}

	if (!activeAttack || manager == nullptr || !manager->IsPlaying())
		return;

	const double songTime = manager->CurrentSongTime();
	UpdateFromSongTime(songTime);

	if (HasMissed(songTime))
	{
		manager->OnPlayerMissed();
		Miss();
	}
}

void RhythmGuardAttackController::OnTriggerEnter2D(Collider2D* collision)
{
	TryResolvePlayerAction(collision);
}

void RhythmGuardAttackController::OnTriggerStay2D(Collider2D* collision)
{
	TryResolvePlayerAction(collision);
}

void RhythmGuardAttackController::Deactivate()
{
	activeAttack = false;
	blockedAttack = false;
	visible = false;
	blockTimerSeconds = 0.0f;
	blockVelocity = Vector2(0.0f, 0.0f);
	gameObject()->SetActive(false);
}

void RhythmGuardAttackController::Block()
{
	activeAttack = false;
	blockedAttack = true;
	visible = true;
	blockDurationSeconds = 0.35f;
	blockTimerSeconds = blockDurationSeconds;
	PrepareBlockMotion();
	ApplyParryVisual();

	gameObject()->SetActive(true);
}

void RhythmGuardAttackController::Miss()
{
	activeAttack = false;
	blockedAttack = true;
	visible = true;
	blockDurationSeconds = 0.35f;
	blockTimerSeconds = blockDurationSeconds;
	blockVelocity = Vector2(0.0f, 0.0f);
	ApplyParryVisual();

	gameObject()->SetActive(true);
}

void RhythmGuardAttackController::UpdateFromSongTime(double songTime)
{
	if (!activeAttack)
		return;

	if (songTime < launchTime)
	{
		transform()->SetPosition(startPosition);
		if (spriteRenderer != nullptr)
			spriteRenderer->color = Color(1.0f, 1.0f, 1.0f, 0.0f);
		return;
	}

	if (!visible)
	{
		visible = true;
		ApplyVisual();
	}

	UpdateAnimationFrame(songTime);

	const float rawProgress = std::clamp(static_cast<float>((songTime - launchTime) / std::max(0.001, hitTime - launchTime)), 0.0f, 1.0f);
	const float progress = std::pow(rawProgress, 4.0f);
	transform()->SetPosition(LerpVector(startPosition, targetPosition, progress));
}

bool RhythmGuardAttackController::IsActiveAttack() const
{
	return activeAttack;
}

bool RhythmGuardAttackController::CanBeBlocked(RhythmDirection inputDirection, double songTime) const
{
	return activeAttack && direction == inputDirection && std::abs(HitErrorSeconds(songTime)) <= hitWindowSeconds;
}

bool RhythmGuardAttackController::HasMissed(double songTime) const
{
	return activeAttack && songTime > hitTime + hitWindowSeconds;
}

double RhythmGuardAttackController::HitErrorSeconds(double songTime) const
{
	return songTime - hitTime;
}

RhythmDirection RhythmGuardAttackController::Direction() const
{
	return direction;
}

Vector2 RhythmGuardAttackController::Position()
{
	return transform()->position();
}

bool RhythmGuardAttackController::TryResolvePlayerAction(Collider2D* collision)
{
	if (collision == nullptr || collision->gameObject() == nullptr)
		return false;

	PlayerActionHitboxScript* hitbox = nullptr;
	if (!collision->gameObject()->TryGetComponent(hitbox))
		return false;

	return TryResolvePlayerAction(hitbox);
}

bool RhythmGuardAttackController::TryResolvePlayerAction(PlayerActionHitboxScript* hitbox)
{
	if (hitbox == nullptr || manager == nullptr || !activeAttack)
		return false;

	if (hitbox->ActiveActionKind() != RhythmActionKind::Guard)
		return false;

	const double songTime = manager->CurrentSongTime();
	if (!CanBeBlocked(hitbox->Direction(), songTime))
		return false;

	if (!hitbox->TryConsume(RhythmActionKind::Guard))
		return false;

	manager->OnGuardBlocked(Position());
	Block();
	return true;
}

void RhythmGuardAttackController::ApplyVisual()
{
	if (spriteRenderer == nullptr)
		spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();

	if (spriteRenderer == nullptr)
		return;

	spriteRenderer->useAtlas = true;
	spriteRenderer->sprite = SpriteIndex::attacked_sp;
	spriteRenderer->currentAtlas = SpriteAtlasRect(0, 0, kAttackFrameWidth, kAttackFrameHeight);
	spriteRenderer->flipX = false;
	spriteRenderer->flipY = false;
	spriteRenderer->color = Color(1.0f, 1.0f, 1.0f, 0.95f);
}

void RhythmGuardAttackController::UpdateAnimationFrame(double songTime)
{
	if (spriteRenderer == nullptr)
		spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();

	if (spriteRenderer == nullptr)
		return;

	const double elapsed = std::max(0.0, (songTime - launchTime)*1.5f);
	const int frame = static_cast<int>(elapsed * kAttackFrameRate) % kAttackFrameCount;
	const int column = frame % kAttackFrameColumns;
	const int row = frame / kAttackFrameColumns;
	spriteRenderer->currentAtlas = SpriteAtlasRect(column, row, kAttackFrameWidth, kAttackFrameHeight);
}

void RhythmGuardAttackController::ApplyParryVisual()
{
	if (spriteRenderer == nullptr)
		spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();

	if (spriteRenderer == nullptr)
		return;

	spriteRenderer->useAtlas = true;
	spriteRenderer->sprite = SpriteIndex::parry;
	spriteRenderer->currentAtlas = SpriteAtlasRect(0, 0, kParryFrameSize, kParryFrameSize);
	spriteRenderer->flipX = false;
	spriteRenderer->flipY = false;
	spriteRenderer->color = Color(1.0f, 1.0f, 1.0f, 0.95f);
	transform()->SetScale(Vector2(kParrySpriteScale, kParrySpriteScale));
	transform()->SetLocalAngle(0.0f);
}

void RhythmGuardAttackController::UpdateParryAnimationFrame()
{
	if (spriteRenderer == nullptr)
		spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();

	if (spriteRenderer == nullptr)
		return;

	const float elapsed = std::clamp(blockDurationSeconds - blockTimerSeconds, 0.0f, blockDurationSeconds);
	const float progress = blockDurationSeconds <= 0.0f ? 1.0f : elapsed / blockDurationSeconds;
	const int frame = std::min(kParryFrameCount - 1, static_cast<int>(progress * kParryFrameCount));
	const int column = frame % kParryFrameColumns;
	const int row = frame / kParryFrameColumns;
	spriteRenderer->currentAtlas = SpriteAtlasRect(column, row, kParryFrameSize, kParryFrameSize);
}

void RhythmGuardAttackController::PrepareBlockMotion()
{
	Vector2 awayDirection = startPosition - targetPosition;
	if (awayDirection.LengthSquared() <= 0.0001f)
		awayDirection = RhythmDirectionVector(direction);
	else
		awayDirection.Normalize();

	const Vector2 sideDirection(-awayDirection.y, awayDirection.x);
	const float sideSign = (slotIndex % 2 == 0) ? 1.0f : -1.0f;
	blockVelocity = awayDirection * 1250.0f + sideDirection * (180.0f * sideSign);
}
