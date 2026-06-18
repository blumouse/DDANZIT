#include "Player.h"

#include "DDANZITEngine.h"
#include "Scene.h"
#include "characterAnimation.h"
#include "Game/Manager/RhythmGameManager.h"
#include "Game/Player/PlayerActionHitboxScript.h"

#include <algorithm>
#include <array>
#include <string>

namespace
{
	constexpr float kActionVisibleSeconds = 0.16f;
	constexpr float kChordWaitSeconds = 0.035f;
	constexpr float kActionColliderWidth = 150.0f;
	constexpr float kActionColliderHeight = 150.0f;
	constexpr float kSkillInputLimitSeconds = 0.45f;
	constexpr int kSkillGaugeMax = 16;

	float DeltaSeconds()
	{
		return std::clamp(Time::unscaledDeltaTime() * 0.001f, 0.0f, 0.05f);
	}

	int ActionIndex(RhythmDirection direction)
	{
		return static_cast<int>(direction);
	}

	const char* ActionName(RhythmDirection direction)
	{
		switch (direction)
		{
		case RhythmDirection::Left:
			return "Left";
		case RhythmDirection::Right:
			return "Right";
		case RhythmDirection::Up:
			return "Up";
		case RhythmDirection::UpLeft:
			return "UpLeft";
		case RhythmDirection::UpRight:
			return "UpRight";
		default:
			return "Unknown";
		}
	}

	float ActionAngle(RhythmDirection direction)
	{
		switch (direction)
		{
		case RhythmDirection::Left:
		case RhythmDirection::Right:
			return 0.0f;
		case RhythmDirection::Up:
			return 90.0f;
		case RhythmDirection::UpLeft:
			return 135.0f;
		case RhythmDirection::UpRight:
			return 45.0f;
		default:
			return 0.0f;
		}
	}

	const std::array<std::array<RhythmSkillInput, 3>, 3>& SkillCommands()
	{
		static const std::array<std::array<RhythmSkillInput, 3>, 3> commands = {
			std::array<RhythmSkillInput, 3>{ RhythmSkillInput::Left, RhythmSkillInput::Down, RhythmSkillInput::Right },
			std::array<RhythmSkillInput, 3>{ RhythmSkillInput::Right, RhythmSkillInput::Down, RhythmSkillInput::Left },
			std::array<RhythmSkillInput, 3>{ RhythmSkillInput::Down, RhythmSkillInput::Down, RhythmSkillInput::Down },
		};
		return commands;
	}
}

PlayerController::PlayerController(GameObject* gameObject) : MonoBehavior(gameObject)
{
	EnableAwake();
	EnableUpdate();
}

void PlayerController::Awake()
{
	CacheAnimationClips();
	ChangeAnimation(PlayerAnimationMode::Idle);
}

void PlayerController::Update()
{
	const float deltaSeconds = DeltaSeconds();
	localSeconds += deltaSeconds;

	TickAction(deltaSeconds);
	TickAnimation(deltaSeconds);

	if (manager == nullptr || !manager->IsPlaying() || IsDead())
		return;

	ReadExitCommandInput();
	if (manager == nullptr || !manager->IsPlaying())
		return;

	ReadSkillInput();
	ReadCombatInput();
}

void PlayerController::BindGameManager(RhythmGameManager* manager)
{
	this->manager = manager;
}

void PlayerController::InitializeForRun(int maxHealth)
{
	this->maxHealth = std::max(1, maxHealth);
	currentHealth = this->maxHealth;
	maxSkillGauge = kSkillGaugeMax;
	skillGauge = 0;
	localSeconds = 0.0f;
	actionRemainSeconds.fill(0.0f);
	actionTouchedTarget.fill(false);
	activeActionKinds.fill(RhythmActionKind::Fire);
	whiffCheckRemainSeconds = 0.0f;
	whiffTouchedTarget = false;

	for (int i = 0; i < static_cast<int>(actionColliderObjects.size()); ++i)
		DisableActionCollider(i);

	ClearExitCommandInput();
	ClearSkillInput();
	ClearInputChord();
	ChangeAnimation(PlayerAnimationMode::Idle);
	NotifyHealthChanged();
	NotifySkillGaugeChanged();
}

void PlayerController::SubscribeHealthChanged(void* context, PlayerHealthChangedCallback callback)
{
	healthChangedContext = context;
	healthChangedCallback = callback;
	NotifyHealthChanged();
}

void PlayerController::SubscribeSkillGaugeChanged(void* context, PlayerSkillGaugeChangedCallback callback)
{
	skillGaugeChangedContext = context;
	skillGaugeChangedCallback = callback;
	NotifySkillGaugeChanged();
}

int PlayerController::CurrentHealth() const
{
	return currentHealth;
}

int PlayerController::MaxHealth() const
{
	return maxHealth;
}

int PlayerController::SkillGauge() const
{
	return skillGauge;
}

int PlayerController::MaxSkillGauge() const
{
	return maxSkillGauge;
}

float PlayerController::HealthRatio() const
{
	if (maxHealth <= 0)
		return 0.0f;

	return static_cast<float>(currentHealth) / static_cast<float>(maxHealth);
}

bool PlayerController::IsDead() const
{
	return currentHealth <= 0;
}

bool PlayerController::IsSkillReady() const
{
	return skillGauge >= maxSkillGauge;
}

void PlayerController::TakeDamage(int damage)
{
	if (damage <= 0 || IsDead())
		return;

	currentHealth = std::max(0, currentHealth - damage);
	NotifyHealthChanged();
	ChangeAnimation(IsDead() ? PlayerAnimationMode::Dead : PlayerAnimationMode::Dodge);
}

void PlayerController::Heal(int amount)
{
	if (amount <= 0 || IsDead())
		return;

	currentHealth = std::min(maxHealth, currentHealth + amount);
	NotifyHealthChanged();
}

void PlayerController::AddSkillGauge(int amount)
{
	if (amount <= 0 || maxSkillGauge <= 0)
		return;

	skillGauge = std::min(maxSkillGauge, skillGauge + amount);
	NotifySkillGaugeChanged();
}

RhythmActionKind PlayerController::CurrentActionKind(RhythmDirection direction) const
{
	return ActiveActionKind(direction);
}

bool PlayerController::TryConsumeActionFromTrigger(RhythmDirection direction, RhythmActionKind actionKind)
{
	return ConsumeAction(direction, actionKind);
}

Vector2 PlayerController::ActionPosition(RhythmDirection direction)
{
	return transform()->position() + RhythmDirectionEffectOffset(direction);
}

void PlayerController::RegisterActionCollider(RhythmDirection direction, GameObject* hitboxObject, BoxCollider2D* collider)
{
	const int index = ActionIndex(direction);
	if (index < 0 || index >= static_cast<int>(actionColliders.size()))
		return;

	actionColliderObjects[index] = hitboxObject;
	actionColliders[index] = collider;
	DisableActionCollider(index);
}

void PlayerController::CacheAnimationClips()
{
	animationClips.clear();

	for (int i = 0; i < transform()->childCount(); ++i)
	{
		Transform* child = transform()->GetChild(i);
		if (child == nullptr || child->gameObject() == nullptr)
			continue;

		characterAnimation* clip = child->gameObject()->GetComponent<characterAnimation>();
		if (clip != nullptr)
			animationClips.push_back(clip);
	}
}

void PlayerController::NotifyHealthChanged()
{
	if (healthChangedCallback != nullptr)
		healthChangedCallback(healthChangedContext, currentHealth, maxHealth);
}

void PlayerController::NotifySkillGaugeChanged()
{
	if (skillGaugeChangedCallback != nullptr)
		skillGaugeChangedCallback(skillGaugeChangedContext, skillGauge, maxSkillGauge);
}

void PlayerController::ReadExitCommandInput()
{
	if (exitCommandBufferCount > 0 && localSeconds - lastExitCommandInputAt > kSkillInputLimitSeconds)
		ClearExitCommandInput();

	if (!Input::GetKeyDown(KeyCode::Escape))
		return;

	if (exitCommandBufferCount == 0)
	{
		exitCommandBufferCount = 1;
		lastExitCommandInputAt = localSeconds;
		return;
	}

	ClearExitCommandInput();
	if (manager != nullptr)
		manager->OnPlayerExitCommand();
}

void PlayerController::ReadSkillInput()
{
	skipCombatInputThisFrame = false;

	if (Input::GetKey(KeyCode::Space))
	{
		ClearSkillInput();
		return;
	}

	if (skillBufferCount > 0 && localSeconds - lastSkillInputAt > kSkillInputLimitSeconds)
		ClearSkillInput();

	const int downCount = CountSkillKeyDowns();
	if (downCount > 1)
	{
		ClearSkillInput();
		return;
	}

	RhythmSkillInput input{};
	if (downCount == 1 && TryReadSkillInput(input))
		QueueSkillInput(input);
}

void PlayerController::ReadCombatInput()
{
	if (skipCombatInputThisFrame)
	{
		ClearInputChord();
		return;
	}

	const bool leftDown = Input::GetKeyDown(KeyCode::LeftArrow) || Input::GetKeyDown(KeyCode::A);
	const bool rightDown = Input::GetKeyDown(KeyCode::RightArrow) || Input::GetKeyDown(KeyCode::D);
	const bool upDown = Input::GetKeyDown(KeyCode::UpArrow) || Input::GetKeyDown(KeyCode::W);

	const bool leftHeld = Input::GetKey(KeyCode::LeftArrow) || Input::GetKey(KeyCode::A);
	const bool rightHeld = Input::GetKey(KeyCode::RightArrow) || Input::GetKey(KeyCode::D);
	const bool upHeld = Input::GetKey(KeyCode::UpArrow) || Input::GetKey(KeyCode::W);
	const bool guardHeld = Input::GetKey(KeyCode::Space);
	const bool guardDown = Input::GetKeyDown(KeyCode::Space);

	if (leftDown || rightDown || upDown || (guardDown && (leftHeld || rightHeld || upHeld)))
		QueueInputChord(leftDown, rightDown, upDown, leftHeld, rightHeld, upHeld, guardHeld);

	if (inputPending && localSeconds >= inputResolveAt)
		ResolveInputChord();
}

void PlayerController::QueueSkillInput(RhythmSkillInput input)
{
	if (skillBufferCount >= static_cast<int>(skillBuffer.size()))
	{
		ClearSkillInput();
		return;
	}

	const int beforeCount = skillBufferCount;
	skillBuffer[skillBufferCount++] = input;
	lastSkillInputAt = localSeconds;

	if (skillBufferCount == 1 && !SkillHasPrefix(skillBufferCount))
	{
		ClearSkillInput();
		return;
	}

	if (skillBufferCount == 1 && manager != nullptr)
		skillComboSnapshot = manager->CurrentCombo();

	if (SkillExact())
	{
		int skillIndex = -1;
		const auto& commands = SkillCommands();
		for (int commandIndex = 0; commandIndex < static_cast<int>(commands.size()); ++commandIndex)
		{
			bool same = true;
			for (int inputIndex = 0; inputIndex < 3; ++inputIndex)
			{
				if (skillBuffer[inputIndex] != commands[commandIndex][inputIndex])
				{
					same = false;
					break;
				}
			}

			if (same)
			{
				skillIndex = commandIndex;
				break;
			}
		}

		skipCombatInputThisFrame = beforeCount > 0;
		ClearSkillInput();

		if (skillIndex >= 0 && IsSkillReady() && manager != nullptr)
		{
			skillGauge = 0;
			NotifySkillGaugeChanged();
			manager->OnPlayerSkill(skillComboSnapshot, skillIndex);
		}
		return;
	}

	if (!SkillHasPrefix(skillBufferCount))
	{
		ClearSkillInput();
		skipCombatInputThisFrame = beforeCount >= 2;
		return;
	}

	if (beforeCount > 0)
		skipCombatInputThisFrame = true;
}

void PlayerController::ClearSkillInput()
{
	skillBufferCount = 0;
	skillComboSnapshot = 0;
}

void PlayerController::ClearExitCommandInput()
{
	exitCommandBufferCount = 0;
	lastExitCommandInputAt = 0.0f;
}

bool PlayerController::SkillHasPrefix(int count) const
{
	if (count < 0 || count > 3)
		return false;

	for (const auto& command : SkillCommands())
	{
		bool matches = true;
		for (int i = 0; i < count; ++i)
		{
			if (skillBuffer[i] != command[i])
			{
				matches = false;
				break;
			}
		}

		if (matches)
			return true;
	}

	return false;
}

bool PlayerController::SkillExact() const
{
	return skillBufferCount == 3 && SkillHasPrefix(3);
}

int PlayerController::CountSkillKeyDowns() const
{
	int count = 0;
	if (Input::GetKeyDown(KeyCode::LeftArrow)) ++count;
	if (Input::GetKeyDown(KeyCode::DownArrow)) ++count;
	if (Input::GetKeyDown(KeyCode::RightArrow)) ++count;
	if (Input::GetKeyDown(KeyCode::UpArrow)) ++count;
	return count;
}

bool PlayerController::TryReadSkillInput(RhythmSkillInput& input) const
{
	if (Input::GetKeyDown(KeyCode::LeftArrow))
	{
		input = RhythmSkillInput::Left;
		return true;
	}
	if (Input::GetKeyDown(KeyCode::DownArrow))
	{
		input = RhythmSkillInput::Down;
		return true;
	}
	if (Input::GetKeyDown(KeyCode::RightArrow))
	{
		input = RhythmSkillInput::Right;
		return true;
	}
	if (Input::GetKeyDown(KeyCode::UpArrow))
	{
		input = RhythmSkillInput::Up;
		return true;
	}

	return false;
}

void PlayerController::QueueInputChord(bool leftDown, bool rightDown, bool upDown, bool leftHeld, bool rightHeld, bool upHeld, bool guardHeld)
{
	if (!inputPending)
	{
		pendingLeft = false;
		pendingRight = false;
		pendingUp = false;
		pendingGuard = false;
		inputPending = true;
		inputResolveAt = localSeconds + kChordWaitSeconds;
	}

	pendingLeft |= leftDown || leftHeld;
	pendingRight |= rightDown || rightHeld;
	pendingUp |= upDown || upHeld;
	pendingGuard |= guardHeld;

	if (HasPendingCombinedInput())
		ResolveInputChord();
}

void PlayerController::ResolveInputChord()
{
	std::vector<RhythmDirection> directions;

	if (pendingUp && pendingRight)
	{
		AddDirection(directions, RhythmDirection::Up);
		AddDirection(directions, RhythmDirection::Right);
		AddDirection(directions, RhythmDirection::UpRight);
	}

	if (pendingUp && pendingLeft)
	{
		AddDirection(directions, RhythmDirection::Up);
		AddDirection(directions, RhythmDirection::Left);
		AddDirection(directions, RhythmDirection::UpLeft);
	}

	if (!HasPendingDiagonal())
	{
		if (pendingLeft) AddDirection(directions, RhythmDirection::Left);
		if (pendingRight) AddDirection(directions, RhythmDirection::Right);
		if (pendingUp) AddDirection(directions, RhythmDirection::Up);
	}

	const bool guardInput = pendingGuard;
	ClearInputChord();

	if (!guardInput && !directions.empty())
	{
		whiffCheckRemainSeconds = kActionVisibleSeconds;
		whiffTouchedTarget = false;
	}

	for (RhythmDirection direction : directions)
		StartAction(direction, guardInput ? RhythmActionKind::Guard : RhythmActionKind::Fire);
}

void PlayerController::ClearInputChord()
{
	inputPending = false;
	pendingLeft = false;
	pendingRight = false;
	pendingUp = false;
	pendingGuard = false;
}

bool PlayerController::HasPendingDiagonal() const
{
	return pendingUp && (pendingLeft || pendingRight);
}

bool PlayerController::HasPendingCombinedInput() const
{
	return HasPendingDiagonal() || (pendingLeft && pendingRight);
}

void PlayerController::AddDirection(std::vector<RhythmDirection>& directions, RhythmDirection direction) const
{
	if (std::find(directions.begin(), directions.end(), direction) == directions.end())
		directions.push_back(direction);
}

void PlayerController::StartAction(RhythmDirection direction, RhythmActionKind actionKind)
{
	const int index = ActionIndex(direction);
	if (index < 0 || index >= static_cast<int>(actionRemainSeconds.size()))
		return;

	//시작점: 플레이어 공격/방어 판정
	activeActionKinds[index] = actionKind;
	actionRemainSeconds[index] = kActionVisibleSeconds;
	actionTouchedTarget[index] = false;

	if (actionColliderObjects[index] != nullptr)
	{
		actionColliderObjects[index]->transform()->SetPosition(ActionPosition(direction));
		actionColliderObjects[index]->transform()->SetAngle(ActionAngle(direction));
		actionColliderObjects[index]->SetActive(true);
	}

	ChangeAnimation(actionKind == RhythmActionKind::Guard ? PlayerAnimationMode::Attack2 : PlayerAnimationMode::Attack);

	if (manager != nullptr)
		manager->OnPlayerAction(direction, actionKind, ActionPosition(direction));
}

void PlayerController::TickAction(float deltaSeconds)
{
	for (int i = 0; i < static_cast<int>(actionRemainSeconds.size()); ++i)
	{
		if (actionRemainSeconds[i] <= 0.0f)
			continue;

		actionRemainSeconds[i] -= deltaSeconds;
		if (actionRemainSeconds[i] <= 0.0f)
			DisableActionCollider(i);
	}

	if (whiffCheckRemainSeconds <= 0.0f)
		return;

	whiffCheckRemainSeconds -= deltaSeconds;
	if (whiffCheckRemainSeconds <= 0.0f)
	{
		if (!whiffTouchedTarget && manager != nullptr)
			manager->OnPlayerWhiff();

		whiffCheckRemainSeconds = 0.0f;
		whiffTouchedTarget = false;
	}
}

void PlayerController::DisableActionCollider(int actionIndex)
{
	if (actionIndex < 0 || actionIndex >= static_cast<int>(actionRemainSeconds.size()))
		return;

	//종료: 플레이어 공격/방어 판정
	actionRemainSeconds[actionIndex] = 0.0f;
	actionTouchedTarget[actionIndex] = false;

	if (actionColliderObjects[actionIndex] != nullptr)
		actionColliderObjects[actionIndex]->SetActive(false);
}

RhythmActionKind PlayerController::ActiveActionKind(RhythmDirection direction) const
{
	const int index = ActionIndex(direction);
	if (index < 0 || index >= static_cast<int>(activeActionKinds.size()))
		return RhythmActionKind::Fire;

	return activeActionKinds[index];
}

bool PlayerController::ConsumeAction(RhythmDirection direction, RhythmActionKind actionKind)
{
	const int index = ActionIndex(direction);
	if (index < 0 || index >= static_cast<int>(actionRemainSeconds.size()))
		return false;

	if (actionRemainSeconds[index] <= 0.0f || activeActionKinds[index] != actionKind)
		return false;

	actionTouchedTarget[index] = true;
	if (actionKind == RhythmActionKind::Fire)
		whiffTouchedTarget = true;

	DisableActionCollider(index);
	return true;
}

void PlayerController::ChangeAnimation(PlayerAnimationMode nextMode)
{
	animationMode = nextMode;
	animationTimerSeconds = 0.0f;
	ResetAnimationFrames();

	switch (animationMode)
	{
	case PlayerAnimationMode::Idle:
		SetFrameDuration(3000.0f);
		SetAnimationTint(1.0f, 1.0f, 1.0f);
		break;
	case PlayerAnimationMode::Attack:
		SetFrameDuration(220.0f);
		SetAnimationTint(1.0f, 0.96f, 0.78f);
		break;
	case PlayerAnimationMode::Attack2:
		SetFrameDuration(220.0f);
		SetAnimationTint(0.70f, 0.90f, 1.0f);
		break;
	case PlayerAnimationMode::Dodge:
		SetFrameDuration(260.0f);
		SetAnimationTint(1.0f, 0.55f, 0.55f);
		break;
	case PlayerAnimationMode::Dead:
		SetFrameDuration(1800.0f);
		SetAnimationTint(0.45f, 0.45f, 0.55f);
		break;
	}
}

void PlayerController::TickAnimation(float deltaSeconds)
{
	animationTimerSeconds += deltaSeconds;

	if ((animationMode == PlayerAnimationMode::Attack || animationMode == PlayerAnimationMode::Attack2) && animationTimerSeconds >= 0.22f)
	{
		//종료: 플레이어 공격 애니메이션
		ChangeAnimation(PlayerAnimationMode::Idle);
	}
	else if (animationMode == PlayerAnimationMode::Dodge && animationTimerSeconds >= 0.26f)
	{
		ChangeAnimation(PlayerAnimationMode::Idle);
	}
}

void PlayerController::SetAnimationTint(float r, float g, float b)
{
	for (characterAnimation* clip : animationClips)
	{
		if (clip != nullptr && clip->spRender != nullptr)
			clip->spRender->color = Color(r, g, b, 1.0f);
	}
}

void PlayerController::SetFrameDuration(float milliseconds)
{
	for (characterAnimation* clip : animationClips)
	{
		if (clip != nullptr)
			clip->SetDuration(milliseconds);
	}
}

void PlayerController::ResetAnimationFrames()
{
	for (characterAnimation* clip : animationClips)
	{
		if (clip == nullptr || clip->spRender == nullptr)
			continue;

		clip->elapsedTime = 0.0f;
		clip->spRender->currentAtlas.pixel_OffsetX = 0;
		clip->spRender->currentAtlas.pixel_OffsetY = 0;
	}
}

Player::Player(Scene* scene) : Draw2D(this)
{
	AddComponent<Rigidbody2D>();
	CircleCollider2D* collider = AddComponent<CircleCollider2D>();
	collider->SetRadius(120.0f);
	PlayerController* controller = AddComponent<PlayerController>();

	for (int i = 0; i < 5; ++i)
	{
		const RhythmDirection direction = static_cast<RhythmDirection>(i);
		GameObject* hitboxObject = scene->hierarchy.AddGameObject();
		hitboxObject->name() = std::string("PlayerHitbox_") + ActionName(direction);
		hitboxObject->transform()->SetParent(transform());
		hitboxObject->transform()->SetPosition(transform()->position() + RhythmDirectionEffectOffset(direction));
		hitboxObject->transform()->SetScale(Vector2(1.0f, 1.0f));
		hitboxObject->transform()->SetAngle(ActionAngle(direction));

		hitboxObject->AddComponent<Rigidbody2D>();
		BoxCollider2D* actionCollider = hitboxObject->AddComponent<BoxCollider2D>();
		actionCollider->SetSize(Vector2(kActionColliderWidth, kActionColliderHeight));

		PlayerActionHitboxScript* hitboxScript = hitboxObject->AddComponent<PlayerActionHitboxScript>();
		hitboxScript->Bind(controller, direction);

		hitboxObject->SetActive(false);
		controller->RegisterActionCollider(direction, hitboxObject, actionCollider);
	}
}

GameObject* Player::Clone() const
{
	return new Player(*this);
}
