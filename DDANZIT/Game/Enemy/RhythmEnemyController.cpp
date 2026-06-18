#include "Game/Enemy/RhythmEnemyController.h"

#include "DDANZITEngine.h"
#include "Game/Manager/RhythmGameManager.h"
#include "Game/Player/PlayerActionHitboxScript.h"

#include <algorithm>
#include <cmath>

namespace
{
	constexpr int kParryFrameColumns = 8;
	constexpr int kParryFrameRows = 3;
	constexpr int kParryFrameCount = kParryFrameColumns * kParryFrameRows;
	constexpr int kParryFrameSize = 256;
	constexpr float kParrySpriteScale = 700.f;

	Vector2 LerpVector(const Vector2& from, const Vector2& to, float t)
	{
		return from + (to - from) * t;
	}
}

RhythmEnemyController::RhythmEnemyController(GameObject* gameObject) : MonoBehavior(gameObject)
{
	EnableAwake();
	EnableUpdate();
	EnableOnTriggerEnter2D();
	EnableOnTriggerStay2D();
}

void RhythmEnemyController::Awake()
{
	spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();
	Deactivate();
}

void RhythmEnemyController::InitializePoolSlot(int slotIndex)
{
	this->slotIndex = slotIndex;
	Deactivate();
}

void RhythmEnemyController::Activate(
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
	RhythmGameManager* manager)
{
	this->manager = manager;
	this->direction = direction;
	this->startPosition = startPosition;
	this->targetPosition = targetPosition;
	this->spawnTime = spawnTime;
	this->hitTime = hitTime;
	this->hitWindowSeconds = hitWindowSeconds;
	this->secondsPerBeat = std::max(0.001f, secondsPerBeat);
	this->movePortion = std::clamp(movePortion, 0.05f, 1.0f);
	this->travelBeats = std::max(1, travelBeats);
	activeEnemy = true;
	dyingEnemy = false;
	missedEnemy = false;
	approachLightShown = false;
	deathTimerSeconds = 0.0f;
	deathVelocity = Vector2(0.0f, 0.0f);
	missTimerSeconds = 0.0f;
	missDurationSeconds = 0.0f;
	currentWalkFrame = -1;
	stepPoints.clear();

	const int stepCount = this->travelBeats;
	const float power = std::max(1.0f, movePower);
	for (int i = 0; i <= stepCount; ++i)
	{
		const float normalizedStep = static_cast<float>(i) / static_cast<float>(stepCount);
		const float ratio = std::pow(normalizedStep, power);
		stepPoints.push_back(LerpVector(startPosition, targetPosition, ratio));
	}

	transform()->SetPosition(startPosition);
	transform()->SetScale(this->travelBeats == 2 ? Vector2(0.6f, 0.6f) : Vector2(0.8f, 0.8f));
	ApplyVisual();
	//시작점: 적 워킹 애니메이션
	ApplyWalkFrame(0);
	gameObject()->SetActive(true);
}

void RhythmEnemyController::Update()
{
	if (missedEnemy)
	{
		const float deltaSeconds = std::clamp(Time::deltaTime(), 0.0f, 0.05f);
		missTimerSeconds -= deltaSeconds;
		UpdateParryAnimationFrame();

		if (spriteRenderer != nullptr)
			spriteRenderer->color = Color(1.0f, 1.0f, 1.0f, std::clamp(missTimerSeconds / missDurationSeconds, 0.0f, 0.95f));

		if (missTimerSeconds <= 0.0f)
			Deactivate();
		return;
	}

	if (dyingEnemy)
	{
		const float deltaSeconds = std::clamp(Time::deltaTime() , 0.0f, 0.05f);
		transform()->SetPosition(transform()->position() + deathVelocity * deltaSeconds);
		deathVelocity = LerpVector(deathVelocity, Vector2(0.0f, 0.0f), std::clamp(deltaSeconds * 8.0f, 0.0f, 1.0f));

		deathTimerSeconds -= deltaSeconds;
		if (deathTimerSeconds <= 0.0f)
		{
			//종료: 적 사망 애니메이션과 오브젝트 표시
			Deactivate();
		}
		return;
	}

	if (!activeEnemy || manager == nullptr || !manager->IsPlaying())
		return;

	const double songTime = manager->CurrentSongTime();
	UpdateFromSongTime(songTime);

	Vector2 lightPosition;
	bool fastLight = false;
	if (TryConsumeApproachLight(songTime, lightPosition, fastLight))
		manager->ShowApproachLightFromEnemy(lightPosition, fastLight);

	if (HasMissed(songTime))
	{
		manager->OnPlayerMissed();
		Miss();
	}
}

void RhythmEnemyController::OnTriggerEnter2D(Collider2D* collision)
{
	TryResolvePlayerAction(collision);
}

void RhythmEnemyController::OnTriggerStay2D(Collider2D* collision)
{
	TryResolvePlayerAction(collision);
}

void RhythmEnemyController::Deactivate()
{
	activeEnemy = false;
	dyingEnemy = false;
	missedEnemy = false;
	approachLightShown = false;
	deathTimerSeconds = 0.0f;
	deathVelocity = Vector2(0.0f, 0.0f);
	missTimerSeconds = 0.0f;
	missDurationSeconds = 0.0f;
	currentWalkFrame = -1;
	//종료: 적 오브젝트 표시
	gameObject()->SetActive(false);
}

void RhythmEnemyController::Kill()
{
	//시작점: 적 사망 애니메이션
	activeEnemy = false;
	dyingEnemy = true;
	missedEnemy = false;
	deathTimerSeconds = 0.8f;
	PrepareDeathMotion();

	if (spriteRenderer == nullptr)
		spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();

	if (spriteRenderer != nullptr)
		spriteRenderer->color = Color(1.0f, 0.15f, 0.1f, 0.95f);
}

void RhythmEnemyController::Miss()
{
	activeEnemy = false;
	dyingEnemy = false;
	missedEnemy = true;
	missDurationSeconds = 0.35f;
	missTimerSeconds = missDurationSeconds;
	ApplyParryVisual();

	gameObject()->SetActive(true);
}

void RhythmEnemyController::UpdateFromSongTime(double songTime)
{
	if (!activeEnemy)
		return;

	if (stepPoints.empty())
	{
		ApplyWalkFrame(0);
		transform()->SetPosition(targetPosition);
		return;
	}

	if (songTime <= spawnTime)
	{
		ApplyWalkFrame(0);
		transform()->SetPosition(stepPoints[0]);
		return;
	}

	Vector2 position = stepPoints.back();
	for (int step = 1; step < static_cast<int>(stepPoints.size()); ++step)
	{
		const double stepEnd = spawnTime + step * secondsPerBeat;
		const double stepStart = stepEnd - secondsPerBeat * movePortion;

		if (songTime < stepStart)
		{
			ApplyWalkFrame((step - 1) % 3);
			transform()->SetPosition(stepPoints[step - 1]);
			return;
		}

		if (songTime <= stepEnd)
		{
			ApplyWalkFrame(step % 3);
			const float ratio = std::clamp(static_cast<float>((songTime - stepStart) / (stepEnd - stepStart)), 0.0f, 1.0f);
			transform()->SetPosition(LerpVector(stepPoints[step - 1], stepPoints[step], ratio));
			return;
		}

		position = stepPoints[step];
	}

	ApplyWalkFrame((static_cast<int>(stepPoints.size()) - 1) % 3);
	transform()->SetPosition(position);
}

bool RhythmEnemyController::IsActiveEnemy() const
{
	return activeEnemy || dyingEnemy || missedEnemy;
}

bool RhythmEnemyController::IsVulnerableEnemy() const
{
	return activeEnemy;
}

bool RhythmEnemyController::CanBeHit(RhythmDirection inputDirection, double songTime) const
{
	return activeEnemy && direction == inputDirection && std::abs(HitErrorSeconds(songTime)) <= hitWindowSeconds;
}

bool RhythmEnemyController::HasMissed(double songTime) const
{
	return activeEnemy && songTime > hitTime + hitWindowSeconds;
}

//적 빛 설정
bool RhythmEnemyController::TryConsumeApproachLight(double songTime, Vector2& lightPosition, bool& fastLight)
{
	if (!activeEnemy || approachLightShown || travelBeats < 2)
		return false;

	const int lightStep = travelBeats == 2 ? 1 : 2;
	const double lightTime = spawnTime + secondsPerBeat * lightStep;
	if (songTime < lightTime)
		return false;

	approachLightShown = true;
	lightPosition = transform()->position();
	fastLight = travelBeats == 2;
	return true;
}

double RhythmEnemyController::HitErrorSeconds(double songTime) const
{
	return songTime - hitTime;
}

RhythmDirection RhythmEnemyController::Direction() const
{
	return direction;
}

Vector2 RhythmEnemyController::Position()
{
	return transform()->position();
}

bool RhythmEnemyController::TryResolvePlayerAction(Collider2D* collision)
{
	if (collision == nullptr || collision->gameObject() == nullptr)
		return false;

	PlayerActionHitboxScript* hitbox = nullptr;
	if (!collision->gameObject()->TryGetComponent(hitbox))
		return false;

	return TryResolvePlayerAction(hitbox);
}

bool RhythmEnemyController::TryResolvePlayerAction(PlayerActionHitboxScript* hitbox)
{
	if (hitbox == nullptr || manager == nullptr || !activeEnemy)
		return false;

	if (hitbox->ActiveActionKind() != RhythmActionKind::Fire)
		return false;

	const double songTime = manager->CurrentSongTime();
	if (!CanBeHit(hitbox->Direction(), songTime))
		return false;

	if (!hitbox->TryConsume(RhythmActionKind::Fire))
		return false;

	manager->OnEnemyHit(*this, songTime);
	Kill();
	return true;
}

void RhythmEnemyController::ApplyVisual()
{
	if (spriteRenderer == nullptr)
		spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();

	if (spriteRenderer == nullptr)
		return;

	spriteRenderer->useAtlas = true;
	spriteRenderer->sprite = SpriteIndex::enemyWalk;
	spriteRenderer->flipX = RhythmShouldFlipForDirection(direction);
	spriteRenderer->flipY = false;
	spriteRenderer->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	ApplyWalkFrame(0);
}

void RhythmEnemyController::ApplyWalkFrame(int frameIndex)
{
	if (spriteRenderer == nullptr)
		spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();

	if (spriteRenderer == nullptr)
		return;

	const int normalizedFrame = (frameIndex % 3);
	if (currentWalkFrame == normalizedFrame)
		return;

	currentWalkFrame = normalizedFrame;
	spriteRenderer->currentAtlas = SpriteAtlasRect(normalizedFrame, 0, 1098, 1143);
}

void RhythmEnemyController::ApplyParryVisual()
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

void RhythmEnemyController::UpdateParryAnimationFrame()
{
	if (spriteRenderer == nullptr)
		spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();

	if (spriteRenderer == nullptr)
		return;

	const float elapsed = std::clamp(missDurationSeconds - missTimerSeconds, 0.0f, missDurationSeconds);
	const float progress = missDurationSeconds <= 0.0f ? 1.0f : elapsed / missDurationSeconds;
	const int frame = std::min(kParryFrameCount - 1, static_cast<int>(progress * kParryFrameCount));
	const int column = frame % kParryFrameColumns;
	const int row = frame / kParryFrameColumns;
	spriteRenderer->currentAtlas = SpriteAtlasRect(column, row, kParryFrameSize, kParryFrameSize);
}

void RhythmEnemyController::PrepareDeathMotion()
{
	Vector2 pathDirection = targetPosition - startPosition;
	if (pathDirection.LengthSquared() <= 0.0001f)
		pathDirection = RhythmDirectionVector(direction) * -1.0f;
	else
		pathDirection.Normalize();

	//밀리는 거리 증가
	pathDirection *= 2.5;
	
	const Vector2 sideDirection(-pathDirection.y, pathDirection.x);
	const float sideSign = (slotIndex % 2 == 0) ? 1.0f : -1.0f;
	deathVelocity = pathDirection * -920.0f + sideDirection * (260.0f * sideSign);
}
