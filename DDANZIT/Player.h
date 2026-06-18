#pragma once

#include "GameObject.h"
#include "Draw2D.h"
#include "MonoBehavior.h"
#include "Game/Common/RhythmTypes.h"

#include <array>
#include <vector>

class characterAnimation;
class Collider2D;
class BoxCollider2D;
class PlayerController;
class RhythmGameManager;

using PlayerHealthChangedCallback = void(*)(void* context, int currentHealth, int maxHealth);
using PlayerSkillGaugeChangedCallback = void(*)(void* context, int currentGauge, int maxGauge);

enum class PlayerAnimationMode
{
	Idle,
	Attack,
	Attack2,
	Dodge,
	Dead
};

// 플레이어 입력 버퍼, 체력, 스킬 게이지, 애니메이션 상태, 공격 판정을 Player 오브젝트 내부에서 관리합니다.
class PlayerController : public MonoBehavior
{
public:
	PlayerController(GameObject* gameObject);

	void Awake() override;
	void Update() override;

	void BindGameManager(RhythmGameManager* manager);
	void InitializeForRun(int maxHealth);
	void SubscribeHealthChanged(void* context, PlayerHealthChangedCallback callback);
	void SubscribeSkillGaugeChanged(void* context, PlayerSkillGaugeChangedCallback callback);

	int CurrentHealth() const;
	int MaxHealth() const;
	int SkillGauge() const;
	int MaxSkillGauge() const;
	float HealthRatio() const;
	bool IsDead() const;
	bool IsSkillReady() const;

	void TakeDamage(int damage);
	void Heal(int amount);
	void AddSkillGauge(int amount);
	RhythmActionKind CurrentActionKind(RhythmDirection direction) const;
	bool TryConsumeActionFromTrigger(RhythmDirection direction, RhythmActionKind actionKind);
	Vector2 ActionPosition(RhythmDirection direction);
	void RegisterActionCollider(RhythmDirection direction, GameObject* hitboxObject, BoxCollider2D* collider);

private:
	void CacheAnimationClips();
	void NotifyHealthChanged();
	void NotifySkillGaugeChanged();
	void ReadExitCommandInput();
	void ReadSkillInput();
	void ReadCombatInput();
	void ClearExitCommandInput();
	void QueueSkillInput(RhythmSkillInput input);
	void ClearSkillInput();
	bool SkillHasPrefix(int count) const;
	bool SkillExact() const;
	int CountSkillKeyDowns() const;
	bool TryReadSkillInput(RhythmSkillInput& input) const;

	void QueueInputChord(bool leftDown, bool rightDown, bool upDown, bool leftHeld, bool rightHeld, bool upHeld, bool guardHeld);
	void ResolveInputChord();
	void ClearInputChord();
	bool HasPendingDiagonal() const;
	bool HasPendingCombinedInput() const;
	void AddDirection(std::vector<RhythmDirection>& directions, RhythmDirection direction) const;
	void StartAction(RhythmDirection direction, RhythmActionKind actionKind);
	void TickAction(float deltaSeconds);
	void DisableActionCollider(int actionIndex);
	RhythmActionKind ActiveActionKind(RhythmDirection direction) const;
	bool ConsumeAction(RhythmDirection direction, RhythmActionKind actionKind);

	void ChangeAnimation(PlayerAnimationMode nextMode);
	void TickAnimation(float deltaSeconds);
	void SetAnimationTint(float r, float g, float b);
	void SetFrameDuration(float milliseconds);
	void ResetAnimationFrames();

	RhythmGameManager* manager = nullptr;
	std::vector<characterAnimation*> animationClips;
	PlayerHealthChangedCallback healthChangedCallback = nullptr;
	void* healthChangedContext = nullptr;
	PlayerSkillGaugeChangedCallback skillGaugeChangedCallback = nullptr;
	void* skillGaugeChangedContext = nullptr;

	int maxHealth = 100;
	int currentHealth = 100;
	int maxSkillGauge = 8;
	int skillGauge = 0;
	int skillComboSnapshot = 0;
	int exitCommandBufferCount = 0;
	int skillBufferCount = 0;
	std::array<RhythmSkillInput, 8> skillBuffer = {};

	bool inputPending = false;
	bool pendingLeft = false;
	bool pendingRight = false;
	bool pendingUp = false;
	bool pendingGuard = false;
	bool skipCombatInputThisFrame = false;

	float lastSkillInputAt = 0.0f;
	float lastExitCommandInputAt = 0.0f;
	float inputResolveAt = 0.0f;
	float localSeconds = 0.0f;
	std::array<float, 5> actionRemainSeconds = {};
	std::array<bool, 5> actionTouchedTarget = {};
	std::array<RhythmActionKind, 5> activeActionKinds = {};
	std::array<GameObject*, 5> actionColliderObjects = {};
	std::array<BoxCollider2D*, 5> actionColliders = {};
	float whiffCheckRemainSeconds = 0.0f;
	bool whiffTouchedTarget = false;
	PlayerAnimationMode animationMode = PlayerAnimationMode::Idle;
	float animationTimerSeconds = 0.0f;
};

class Player : public GameObject, public Draw2D
{
public:
	Player(Scene* scene);

	Player(const Player& other) = default;

	GameObject* Clone() const override;

	~Player() = default;



};

