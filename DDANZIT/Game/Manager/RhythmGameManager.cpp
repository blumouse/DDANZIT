#include "Game/Manager/RhythmGameManager.h"

#include "DDANZITEngine.h"
#include "BeatMediaControl.h"
#include "Debug.h"
#include "Text.h"
#include "Game/UI/HealthBarObserver.h"
#include "Player.h"
#include "Game/Effect/RhythmEffectController.h"
#include "Game/Enemy/RhythmEnemyController.h"
#include "Game/GuardAttack/RhythmGuardAttackController.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <sstream>

namespace
{
	constexpr int kEnemyPoolSize = 64;
	constexpr int kGuardAttackPoolSize = 32;
	constexpr int kEffectPoolSize = 96;
	constexpr int kPlayerMaxHealth = 100;
	constexpr float kActionVisibleSeconds = 0.12f;
	constexpr float kLightTime = 0.2f;
	constexpr float kLightAlpha = 6.f;
	constexpr float kLightWidth = 8.f;
	constexpr float kShakeTime = 0.4f;
	constexpr float kShakePower = 10.0f;
	constexpr float kPunchTime = 0.11f;
	constexpr float kPunchScale = 1.16f;
	constexpr float kComboBonus = 0.01f;
	constexpr int kComboCap = 30;
	constexpr float kRhythmPi = 3.14159265f;
	constexpr float kSpawnLookAhead = 0.05f;
	constexpr float kSongPreviewVolume = 0.7f;
	constexpr float kSongPreviewFadeSeconds = 0.35f;
	constexpr float kSongPreviewStaticSeconds = 1.0f;
	constexpr float kGameplayMusicVolume = 0.7f;
	constexpr float kSkillVideoMusicVolume = 0.5f;
	constexpr float kSkillAutoClearSeconds = 8.0f;
	constexpr double kSongEndNoThreatSeconds = 2.0f;
	constexpr float kSkillVideoEndFrameHoldSeconds = 0.16f;
	constexpr float kUltiCommandAnimationSeconds = 10.0f;
	constexpr int kUltiCommandFrameCount = 3;
	constexpr int kUltiCommandFrameWidth = 690;
	constexpr int kUltiCommandFrameHeight = 216;
	const Vector2 kStatusTextDefaultPosition = Vector2(0.0f, 400.0f);
	const Vector2 kStatusTextDefaultScale = Vector2(1180.0f, 70.0f);
	const Vector2 kStatusTextPlayingPosition = Vector2(560.0f, 0.0f);
	const Vector2 kStatusTextPlayingScale = Vector2(360.0f, 120.0f);

	SFXIndex FinishSfxForVideo(VideoIndex video)
	{
		switch (video)
		{
		case VideoIndex::Aria:
			return SFXIndex::ariaUltimateFinish;
		case VideoIndex::GungWoo:
			return SFXIndex::wooUltimateFinish;
		case VideoIndex::Suna:
			return SFXIndex::sunaUltimateFinish;
		default:
			return SFXIndex::None;
		}
	}

	int DirectionIndex(RhythmDirection direction)
	{
		switch (direction)
		{
		case RhythmDirection::Left:
			return 0;
		case RhythmDirection::Right:
			return 1;
		case RhythmDirection::Up:
			return 2;
		case RhythmDirection::UpLeft:
			return 3;
		case RhythmDirection::UpRight:
			return 4;
		default:
			return 0;
		}
	}

	float ClampFloat(float value, float minValue, float maxValue)
	{
		return std::clamp(value, minValue, maxValue);
	}

	Vector2 LerpVector(const Vector2& from, const Vector2& to, float t)
	{
		return from + (to - from) * t;
	}

	Color LerpColor(const Color& from, const Color& to, float t)
	{
		return Color(
			from.r + (to.r - from.r) * t,
			from.g + (to.g - from.g) * t,
			from.b + (to.b - from.b) * t,
			from.a + (to.a - from.a) * t);
	}

	void SetSpriteColor(SpriteRenderer* sprite, const Color& color)
	{
		if (sprite != nullptr)
			sprite->color = color;
	}
}

RhythmGameManager::RhythmGameManager(GameObject* gameObject) : MonoBehavior(gameObject)
{
	EnableStart();
	EnableUpdate();
}

void RhythmGameManager::Start()
{
	InitializeSceneObjects();
	ChangeToSongSelect();
}

void RhythmGameManager::Update()
{
	BeatMediaSystem::Instance().UpdateMp3Fades(UnscaledDeltaSeconds());

	//상태머신은 상태 전환의 Enter/Exit만 담당하고, 프레임 진행은 GameManager 오브젝트의 Update에서 직접 분기합니다.
	if (stateMachine.CurrentState() == &songSelectState)
		UpdateSongSelect();
	else if (stateMachine.CurrentState() == &prepareState)
		UpdatePrepare();
	else if (stateMachine.CurrentState() == &playingState)
		UpdatePlaying();
	else if (stateMachine.CurrentState() == &resultState)
		UpdateResult();
}

void RhythmGameManager::EnterSongSelect()
{
	BeatMediaSystem::Instance().StopAllMp3();
	BeatMediaSystem::Instance().StopAllSfx();
	BeatMediaSystem::Instance().StopAllMp4();
	DeactivateAllEnemies();
	DeactivateAllGuardAttacks();
	DeactivateAllEffects();
	InitializePlayerForRun();

	endedByDeath = false;
	musicPlaybackStarted = false;
	songEndCleanupDone = false;
	skillVideoActive = false;
	skillVideoFinishSfxPlayed = false;
	skillVideoTimerSeconds = 0.0f;
	skillVideoEndHoldSeconds = 0.0f;
	activeSkillEffect = VideoIndex::None;
	skillEffectRemainingSeconds = 0.0f;
	stateTimerSeconds = 0.0f;
	gameSeconds = 0.0f;
	currentSongTime = 0.0;
	difficultySelectActive = false;
	activePreviewMusic = MusicIndex::choseMusic;
	pendingPreviewMusic = MusicIndex::None;
	previewStaticTimerSeconds = 0.0f;
	BeatMediaSystem::Instance().SetMp3Volume(MusicIndex::choseMusic, 0.65f);
	BeatMediaSystem::Instance().PlayMp3(MusicIndex::choseMusic, true);

	SetSelectionUiVisible(true);
	SetGameplayUiVisible(false);
	RefreshSelectionUi();
	WriteDebugLine(L"[SongSelect] Choose song and difficulty.");
}

void RhythmGameManager::UpdateSongSelect()
{
	UpdateSongPreview(UnscaledDeltaSeconds());

	if (!difficultySelectActive)
	{
		const KeyCode songKeys[] = { KeyCode::Num1, KeyCode::Num2, KeyCode::Num3, KeyCode::Num4, KeyCode::Num5, KeyCode::Num6, KeyCode::Num7 };
		const int songCount = GetRhythmSongCount();
		const int directKeyCount = std::min(songCount, static_cast<int>(sizeof(songKeys) / sizeof(songKeys[0])));
		int nextSongIndex = selectedSongIndex;
		bool openDifficulty = false;
		bool changedSong = false;

		for (int i = 0; i < directKeyCount; ++i)
		{
			if (!Input::GetKeyDown(songKeys[i]))
				continue;

			nextSongIndex = i;
			openDifficulty = true;
			changedSong = true;
			break;
		}

		if (!changedSong && Input::GetKeyDown(KeyCode::LeftArrow))
		{
			nextSongIndex = (selectedSongIndex + songCount - 1) % songCount;
			changedSong = true;
		}
		else if (!changedSong && Input::GetKeyDown(KeyCode::RightArrow))
		{
			nextSongIndex = (selectedSongIndex + 1) % songCount;
			changedSong = true;
		}

		if (changedSong)
		{
			const MusicIndex previewMusic = GetRhythmSong(nextSongIndex).highlightMusicId;
			const bool shouldPreview = selectedSongIndex != nextSongIndex || activePreviewMusic != previewMusic;
			selectedSongIndex = nextSongIndex;

			if (shouldPreview)
				QueueSongPreview(selectedSongIndex);

			if (openDifficulty)
				difficultySelectActive = true;

			BeatMediaSystem::Instance().PlaySfx(SFXIndex::tvUiMoved, 0.85f, false);
			RefreshSelectionUi();
		}
		else if (Input::GetKeyDown(KeyCode::Space))
		{
			difficultySelectActive = true;
			BeatMediaSystem::Instance().PlaySfx(SFXIndex::tvUiMoved, 0.85f, false);
			RefreshSelectionUi();
		}

		return;
	}

	if (Input::GetKeyDown(KeyCode::Escape))
	{
		difficultySelectActive = false;
		BeatMediaSystem::Instance().PlaySfx(SFXIndex::tvUiMoved, 0.85f, false);
		RefreshSelectionUi();
		return;
	}

	if (Input::GetKeyDown(KeyCode::Num3))
	{
		if (selectedDifficultyIndex != 0)
		{
			selectedDifficultyIndex = 0;
			BeatMediaSystem::Instance().PlaySfx(SFXIndex::tvUiMoved, 0.85f, false);
			RefreshSelectionUi();
		}
	}
	else if (Input::GetKeyDown(KeyCode::Num4))
	{
		if (selectedDifficultyIndex != 1)
		{
			selectedDifficultyIndex = 1;
			BeatMediaSystem::Instance().PlaySfx(SFXIndex::tvUiMoved, 0.85f, false);
			RefreshSelectionUi();
		}
	}
	else if (Input::GetKeyDown(KeyCode::Num5))
	{
		if (selectedDifficultyIndex != 2)
		{
			selectedDifficultyIndex = 2;
			BeatMediaSystem::Instance().PlaySfx(SFXIndex::tvUiMoved, 0.85f, false);
			RefreshSelectionUi();
		}
	}
	else if (Input::GetKeyDown(KeyCode::Q) || Input::GetKeyDown(KeyCode::DownArrow))
	{
		selectedDifficultyIndex = (selectedDifficultyIndex + GetRhythmDifficultyCount() - 1) % GetRhythmDifficultyCount();
		BeatMediaSystem::Instance().PlaySfx(SFXIndex::tvUiMoved, 0.85f, false);
		RefreshSelectionUi();
	}
	else if (Input::GetKeyDown(KeyCode::E) || Input::GetKeyDown(KeyCode::UpArrow))
	{
		selectedDifficultyIndex = (selectedDifficultyIndex + 1) % GetRhythmDifficultyCount();
		BeatMediaSystem::Instance().PlaySfx(SFXIndex::tvUiMoved, 0.85f, false);
		RefreshSelectionUi();
	}

	if (Input::GetKeyDown(KeyCode::Space))
	{
		StopSongPreview();
		ChangeToPrepare();
	}
}

void RhythmGameManager::EnterPrepare()
{
	DeactivateAllEnemies();
	DeactivateAllGuardAttacks();
	DeactivateAllEffects();
	InitializePlayerForRun();

	stateTimerSeconds = 0.0f;
	SetSelectionUiVisible(false);
	SetGameplayUiVisible(true);
	UpdateProgressBar(0.0);
	RefreshGameplayMarkers();

	std::wstringstream stream;
	stream << L"READY - " << GetRhythmSong(selectedSongIndex).title
		<< L" / " << GetRhythmDifficulty(selectedDifficultyIndex).title;
	UpdateStatusText(stream.str());
	WriteDebugLine(stream.str());
}

void RhythmGameManager::UpdatePrepare()
{
	stateTimerSeconds += UnscaledDeltaSeconds();

	std::wstringstream stream;
	stream << L"READY " << std::max(0.0f, 1.5f - stateTimerSeconds);
	UpdateStatusText(stream.str());

	//시작 상태 시간
	if (stateTimerSeconds >= 0.0005f)
		ChangeToPlaying();
}

void RhythmGameManager::EnterPlaying()
{
	DeactivateAllEnemies();
	DeactivateAllGuardAttacks();
	DeactivateAllEffects();

	runStats.Reset();
	endedByDeath = false;
	nextBeat = 0;
	stateTimerSeconds = 0.0f;
	gameSeconds = 0.0f;
	currentSongTime = 0.0;
	musicPlaybackStarted = false;
	songEndCleanupDone = false;
	skillVideoActive = false;
	skillVideoFinishSfxPlayed = false;
	skillVideoTimerSeconds = 0.0f;
	skillVideoDurationSeconds = 0.0f;
	skillVideoEndHoldSeconds = 0.0f;
	activeSkillEffect = VideoIndex::None;
	skillEffectRemainingSeconds = 0.0f;
	attackWarningLights.clear();
	enemyArrivalBeats.clear();
	attackArrivalBeats.clear();
	shakeRemaining = 0.0f;
	punchRemaining = 0.0f;

	enemyRandom.seed(1009u + static_cast<unsigned int>(selectedSongIndex * 97 + selectedDifficultyIndex * 31));
	InitializePlayerForRun();
	if (healthBarObject != nullptr)
		healthBarObject->SetActive(true);

	const RhythmSongConfig& song = GetRhythmSong(selectedSongIndex);
	BeatMediaSystem::Instance().SetMp3Volume(song.musicId, kGameplayMusicVolume);
	BeatMediaSystem::Instance().SetMp3PositionSeconds(song.musicId, 0.0);
	musicPlaybackStarted = BeatMediaSystem::Instance().PlayMp3(song.musicId, true);
	cachedSongLength = CurrentSongLengthSeconds();

	spawnBeatStep = std::max(1, static_cast<int>(std::round(song.spawnEverySeconds / BeatSec())));
	//SpawnOpeningPattern();
	nextBeat = spawnBeatStep;

	//RefreshGameplayMarkers();
	UpdateProgressBar(0.0);
	std::wstringstream stream;
	stream << L"PLAYING - " << song.title
		<< L" / EnemyPool " << enemyPool.size()
		<< L" / GuardPool " << guardAttackPool.size()
		<< (musicPlaybackStarted ? L" / Music" : L" / Music Failed");
	UpdateStatusText(stream.str());
	WriteDebugLine(musicPlaybackStarted ? L"[Playing] Music started." : L"[Playing] Music failed.");
}

void RhythmGameManager::UpdatePlaying()
{
	const float deltaSeconds = DeltaSeconds();
	stateTimerSeconds += deltaSeconds;
	gameSeconds += deltaSeconds;

	const double songTime = SongNow();
	currentSongTime = songTime;

	if (IsSongEndCleanupTime(songTime))
		ClearSongEndThreats();
	else
	{
		ProcessAttackWarningLights(songTime);
		ScheduleSpawns(songTime);
	}

	UpdateSkillVideo(deltaSeconds);
	UpdateSkillEffect(deltaSeconds);
	UpdateUltiCommand(deltaSeconds);
	TickEffects(deltaSeconds);
	UpdateCameraShake(deltaSeconds);
	UpdatePunch(deltaSeconds);
	UpdateProgressBar(songTime);
	CheckGameEnd(songTime);
}

void RhythmGameManager::ExitPlaying()
{
	const RhythmSongConfig& song = GetRhythmSong(selectedSongIndex);
	BeatMediaSystem::Instance().StopMp3(song.musicId);
	StopSkillVideo();
	DeactivateAllEnemies();
	DeactivateAllGuardAttacks();
	attackWarningLights.clear();
}

void RhythmGameManager::EnterResult()
{
	SetSelectionUiVisible(false);
	SetGameplayUiVisible(false);

	if (resultTextObject != nullptr)
		resultTextObject->SetActive(true);

	UpdateResultText();
}

void RhythmGameManager::UpdateResult()
{
	if (Input::GetKeyDown(KeyCode::Space))
		ChangeToSongSelect();
}

void RhythmGameManager::ChangeToSongSelect()
{
	stateMachine.ChangeState(&songSelectState, *this);
}

void RhythmGameManager::ChangeToPrepare()
{
	stateMachine.ChangeState(&prepareState, *this);
}

void RhythmGameManager::ChangeToPlaying()
{
	stateMachine.ChangeState(&playingState, *this);
}

void RhythmGameManager::ChangeToResult()
{
	stateMachine.ChangeState(&resultState, *this);
}

bool RhythmGameManager::IsPlaying()
{
	return stateMachine.CurrentState() == &playingState;
}

int RhythmGameManager::CurrentCombo() const
{
	return runStats.combo;
}

double RhythmGameManager::CurrentSongTime() const
{
	return currentSongTime;
}

void RhythmGameManager::OnPlayerAction(RhythmDirection direction, RhythmActionKind actionKind, const Vector2& position)
{
	(void)direction;
	(void)actionKind;
	(void)position;
}

void RhythmGameManager::OnPlayerWhiff()
{
	RegisterWhiff();
}

void RhythmGameManager::OnPlayerSkill(int comboSnapshot, int skillIndex)
{
	runStats.RestoreComboAtLeast(comboSnapshot);
	ToggleUltiCommand(false);
	StartSkillVideo(skillIndex);
	ResolveSkillTargets(true, true);
	Shake(kShakeTime * 1.3f, kShakePower * 1.4f);
	Punch(kPunchTime * 1.2f, kPunchScale + 0.08f);
}

void RhythmGameManager::OnPlayerExitCommand()
{
	if (!IsPlaying())
		return;

	BeatMediaSystem::Instance().PlaySfx(SFXIndex::tvUiMoved, 0.85f, false);
	ChangeToSongSelect();
}

void RhythmGameManager::OnEnemyHit(RhythmEnemyController& enemy, double songTime)
{
	runStats.RecordHit();
	if (playerController != nullptr && !skillVideoActive)
		playerController->AddSkillGauge(1);
	HitFx(enemy, songTime);
}

void RhythmGameManager::OnGuardBlocked(const Vector2& position)
{
	runStats.RecordBlock();
	if (playerController != nullptr && !skillVideoActive)
		playerController->AddSkillGauge(1);
	BlockFx(position);
}

void RhythmGameManager::OnPlayerMissed()
{
	RegisterMiss();
}

void RhythmGameManager::ShowApproachLightFromEnemy(const Vector2& position, bool fast)
{
	ShowApproachLight(position, fast);
}

void RhythmGameManager::InitializeSceneObjects()
{
	if (sceneInitialized)
		return;

	playerObject = GameObject::Find("Player");
	if (playerObject != nullptr)
	{
		playerController = playerObject->GetComponent<PlayerController>();
		playerBaseScale = playerObject->transform()->scale();
	}

	cameraObject = GameObject::Find("MainCamera");
	if (cameraObject != nullptr)
		cameraBasePosition = cameraObject->transform()->position();

	healthBarObject = GameObject::Find("HealthBarFill");
	if (healthBarObject != nullptr)
		healthBarObserver = healthBarObject->GetComponent<HealthBarObserver>();

	GameObject* status = GameObject::Find("StatusText");
	if (status != nullptr)
		statusText = status->GetComponent<Text>();

	GameObject* result = GameObject::Find("ResultText");
	if (result != nullptr)
		resultText = result->GetComponent<Text>();

	CollectEnemyPool();
	CollectGuardAttackPool();
	CollectEffectPool();
	CollectSelectionUi();
	CollectGameplayUi();

	if (healthBarObserver != nullptr)
		healthBarObserver->SetTarget(playerController);

	if (playerController != nullptr)
	{
		playerController->BindGameManager(this);
		playerController->SubscribeSkillGaugeChanged(this, &RhythmGameManager::OnPlayerSkillGaugeChanged);
	}

	sceneInitialized = true;
}

void RhythmGameManager::InitializePlayerForRun()
{
	if (playerController != nullptr)
		playerController->InitializeForRun(kPlayerMaxHealth);

	if (healthBarObserver != nullptr)
		healthBarObserver->ResetView();

	if (playerController != nullptr)
		UpdateSkillGaugeView(playerController->SkillGauge(), playerController->MaxSkillGauge());

	if (playerObject != nullptr)
		playerObject->transform()->SetScale(playerBaseScale);

	if (cameraObject != nullptr)
		cameraObject->transform()->SetPosition(cameraBasePosition);
}

void RhythmGameManager::CollectEnemyPool()
{
	enemyPool.clear();

	for (int i = 0; i < kEnemyPoolSize; ++i)
	{
		GameObject* enemyObject = GameObject::Find("RhythmEnemy_" + std::to_string(i));
		if (enemyObject == nullptr)
			continue;

		RhythmEnemyController* enemy = enemyObject->GetComponent<RhythmEnemyController>();
		if (enemy == nullptr)
			continue;

		enemy->InitializePoolSlot(i);
		enemyPool.push_back(enemy);
	}
}

void RhythmGameManager::CollectGuardAttackPool()
{
	guardAttackPool.clear();

	for (int i = 0; i < kGuardAttackPoolSize; ++i)
	{
		GameObject* attackObject = GameObject::Find("RhythmGuardAttack_" + std::to_string(i));
		if (attackObject == nullptr)
			continue;

		RhythmGuardAttackController* attack = attackObject->GetComponent<RhythmGuardAttackController>();
		if (attack == nullptr)
			continue;

		attack->InitializePoolSlot(i);
		guardAttackPool.push_back(attack);
	}
}

void RhythmGameManager::CollectEffectPool()
{
	effectPool.clear();

	for (int i = 0; i < kEffectPoolSize; ++i)
	{
		GameObject* effectObject = GameObject::Find("RhythmEffect_" + std::to_string(i));
		if (effectObject == nullptr)
			continue;

		RhythmEffectController* effect = effectObject->GetComponent<RhythmEffectController>();
		if (effect == nullptr)
			continue;

		effect->InitializePoolSlot(i);
		effectPool.push_back(effect);
	}
}

void RhythmGameManager::CollectSelectionUi()
{
	menuStaticObject = GameObject::Find("MenuStaticPanel");
	menuSongPanelObject = GameObject::Find("MenuSongPanel");
	if (menuSongPanelObject != nullptr)
		menuSongPanelSprite = menuSongPanelObject->GetComponent<SpriteRenderer>();

	selectPanelObject = GameObject::Find("SongSelectPanel");
	startHintObject = GameObject::Find("StartHintPanel");
	startHintLabelObject = GameObject::Find("StartHintLabel");
	songTitleObject = GameObject::Find("SongTitleLabel");
	difficultyTitleObject = GameObject::Find("DifficultyTitleLabel");

	for (int i = 0; i < static_cast<int>(songCardObjects.size()); ++i)
	{
		songCardObjects[i] = GameObject::Find("SongCard_" + std::to_string(i));
		songLabelObjects[i] = GameObject::Find("SongLabel_" + std::to_string(i));
		if (songCardObjects[i] != nullptr)
			songCardSprites[i] = songCardObjects[i]->GetComponent<SpriteRenderer>();
	}

	for (int i = 0; i < static_cast<int>(difficultyCardObjects.size()); ++i)
	{
		difficultyCardObjects[i] = GameObject::Find("DifficultyCard_" + std::to_string(i));
		difficultyLabelObjects[i] = GameObject::Find("DifficultyLabel_" + std::to_string(i));
		if (difficultyCardObjects[i] != nullptr)
			difficultyCardSprites[i] = difficultyCardObjects[i]->GetComponent<SpriteRenderer>();
	}

	selectionExtraObjects.clear();

	statusTextObject = GameObject::Find("StatusText");
	resultTextObject = GameObject::Find("ResultText");
}

void RhythmGameManager::CollectGameplayUi()
{
	gameBackgroundObject = GameObject::Find("GameBackground");
	progressTrackObject = GameObject::Find("ProgressTrack");
	progressFillObject = GameObject::Find("ProgressFill");

	if (progressFillObject != nullptr)
	{
		progressFillSprite = progressFillObject->GetComponent<SpriteRenderer>();
		progressFillBasePosition = progressFillObject->transform()->position();
		progressFillBaseScale = progressFillObject->transform()->scale();
	}

	skillGaugeTrackObject = GameObject::Find("SkillGaugeTrack");
	skillGaugeFillObject = GameObject::Find("SkillGaugeFill");
	ultiCommandObject = GameObject::Find("UltiCommand");
	if (ultiCommandObject != nullptr)
		ultiCommandSprite = ultiCommandObject->GetComponent<SpriteRenderer>();

	if (skillGaugeFillObject != nullptr)
	{
		skillGaugeFillSprite = skillGaugeFillObject->GetComponent<SpriteRenderer>();
		skillGaugeFillBasePosition = skillGaugeFillObject->transform()->position();
		skillGaugeFillBaseScale = skillGaugeFillObject->transform()->scale();
	}

	const char* names[] = { "HitTarget_Left", "HitTarget_Right", "HitTarget_Up", "HitTarget_UpLeft", "HitTarget_UpRight" };
	for (int i = 0; i < 5; ++i)
	{
		hitTargetObjects[i] = GameObject::Find(names[i]);
		if (hitTargetObjects[i] != nullptr)
			hitTargetSprites[i] = hitTargetObjects[i]->GetComponent<SpriteRenderer>();
	}
}

void RhythmGameManager::DeactivateAllEnemies()
{
	for (RhythmEnemyController* enemy : enemyPool)
	{
		if (enemy != nullptr)
			enemy->Deactivate();
	}
}

void RhythmGameManager::DeactivateAllGuardAttacks()
{
	for (RhythmGuardAttackController* attack : guardAttackPool)
	{
		if (attack != nullptr)
			attack->Deactivate();
	}
}

void RhythmGameManager::DeactivateAllEffects()
{
	for (RhythmEffectController* effect : effectPool)
	{
		if (effect != nullptr)
			effect->Deactivate();
	}
}

void RhythmGameManager::SetSelectionUiVisible(bool visible)
{
	if (menuStaticObject != nullptr)
		menuStaticObject->SetActive(visible);

	if (menuSongPanelObject != nullptr)
		menuSongPanelObject->SetActive(visible);

	if (!visible)
	{
		SetSongSelectionStage(false);
		if (resultTextObject != nullptr)
			resultTextObject->SetActive(false);
		return;
	}

	SetSongSelectionStage(difficultySelectActive);

	if (resultTextObject != nullptr)
		resultTextObject->SetActive(true);
}

void RhythmGameManager::SetGameplayUiVisible(bool visible)
{
	if (gameBackgroundObject != nullptr)
		gameBackgroundObject->SetActive(visible);

	if (healthBarObject != nullptr)
		healthBarObject->SetActive(false);
	ToggleUltiCommand(false);

	if (progressTrackObject != nullptr)
		progressTrackObject->SetActive(visible);

	if (progressFillObject != nullptr)
		progressFillObject->SetActive(visible);

	if (skillGaugeTrackObject != nullptr)
		skillGaugeTrackObject->SetActive(visible);

	if (skillGaugeFillObject != nullptr)
		skillGaugeFillObject->SetActive(visible);

	for (GameObject* target : hitTargetObjects)
	{
		if (target != nullptr)
			target->SetActive(visible);
	}
}

void RhythmGameManager::RefreshSelectionUi()
{
	if (menuSongPanelSprite != nullptr)
		menuSongPanelSprite->sprite = GetRhythmSong(selectedSongIndex).panelSprite;

	if (menuSongPanelObject != nullptr)
		menuSongPanelObject->SetActive(previewStaticTimerSeconds <= 0.0f && pendingPreviewMusic == MusicIndex::None);

	SetSongSelectionStage(difficultySelectActive);

	for (int i = 0; i < static_cast<int>(songCardSprites.size()); ++i)
	{
		const bool selected = i == selectedSongIndex;
		SetSpriteColor(songCardSprites[i], selected ? Color(0.25f, 0.70f, 1.0f, 0.95f) : Color(0.16f, 0.19f, 0.24f, 0.88f));
	}

	for (int i = 0; i < static_cast<int>(difficultyCardSprites.size()); ++i)
	{
		const bool selected = i == selectedDifficultyIndex;
		SetSpriteColor(difficultyCardSprites[i], selected ? Color(1.0f, 0.78f, 0.24f, 0.95f) : Color(0.18f, 0.20f, 0.23f, 0.88f));
	}

		UpdateResultText();
	if (difficultySelectActive) {
		UpdateStatusText(L"ESC Back\t\t\t\t\tSpace Start");
	}
	else {
		UpdateStatusText(L"Left(← →)Right\t\t\t\t\tSpace Select");
	}

	std::wstringstream result;
	result <<GetRhythmSong(selectedSongIndex).title;
	if (difficultySelectActive) {
		resultTextObject->transform()->SetPosition(Vector2(0, -215));
		result << L" / " << GetRhythmDifficulty(selectedDifficultyIndex).title << L"    ESC returns to song select";
	}
	else {
		resultTextObject->transform()->SetPosition(Vector2(0, -210));
		result << L"\t\t\t\t\t\t\t\t\t\t\t" << GetRhythmSong(selectedSongIndex).title << "\n\n\n\n\n\n\n\n\n\n\n\n\n   Choose a song to open difficulty select";
	}

	if (resultText != nullptr)
		resultText->text = result.str();
}

void RhythmGameManager::SetSongSelectionStage(bool difficultyStage)
{
	if (selectPanelObject != nullptr)
		selectPanelObject->SetActive(difficultyStage);

	if (startHintObject != nullptr)
		startHintObject->SetActive(difficultyStage);

	if (startHintLabelObject != nullptr)
		startHintLabelObject->SetActive(difficultyStage);

	if (songTitleObject != nullptr)
		songTitleObject->SetActive(false);

	if (difficultyTitleObject != nullptr)
		difficultyTitleObject->SetActive(difficultyStage);

	for (GameObject* card : songCardObjects)
	{
		if (card != nullptr)
			card->SetActive(false);
	}

	for (GameObject* label : songLabelObjects)
	{
		if (label != nullptr)
			label->SetActive(false);
	}

	for (GameObject* card : difficultyCardObjects)
	{
		if (card != nullptr)
			card->SetActive(difficultyStage);
	}

	for (GameObject* label : difficultyLabelObjects)
	{
		if (label != nullptr)
			label->SetActive(difficultyStage);
	}

	for (GameObject* extra : selectionExtraObjects)
	{
		if (extra != nullptr)
			extra->SetActive(difficultyStage);
	}
}

void RhythmGameManager::RefreshGameplayMarkers()
{
	for (int i = 0; i < 5; ++i)
	{
		if (hitTargetObjects[i] != nullptr)
			hitTargetObjects[i]->transform()->SetPosition(DirectionTargetPosition(static_cast<RhythmDirection>(i)));
	}

	SetSpriteColor(hitTargetSprites[0], Color(0.35f, 0.65f, 1.0f, 0.35f));
	SetSpriteColor(hitTargetSprites[1], Color(1.0f, 0.45f, 0.35f, 0.35f));
	SetSpriteColor(hitTargetSprites[2], Color(0.45f, 1.0f, 0.65f, 0.35f));
	SetSpriteColor(hitTargetSprites[3], Color(0.55f, 0.75f, 1.0f, 0.35f));
	SetSpriteColor(hitTargetSprites[4], Color(1.0f, 0.65f, 0.45f, 0.35f));
}

void RhythmGameManager::QueueSongPreview(int songIndex)
{
	const RhythmSongConfig& song = GetRhythmSong(songIndex);
	const MusicIndex nextPreview = song.highlightMusicId;

	if (activePreviewMusic != MusicIndex::None)
		BeatMediaSystem::Instance().FadeOutMp3(activePreviewMusic, kSongPreviewFadeSeconds, true);

	pendingPreviewMusic = nextPreview;
	activePreviewMusic = MusicIndex::None;
	previewStaticTimerSeconds = kSongPreviewStaticSeconds;
	BeatMediaSystem::Instance().PlaySfx(SFXIndex::televisionStatic, 0.9f, false);
	RefreshSelectionUi();
}

void RhythmGameManager::UpdateSongPreview(float deltaSeconds)
{
	if (previewStaticTimerSeconds > 0.0f)
	{
		previewStaticTimerSeconds = std::max(0.0f, previewStaticTimerSeconds - deltaSeconds);
		if (previewStaticTimerSeconds <= 0.0f && pendingPreviewMusic != MusicIndex::None)
		{
			activePreviewMusic = pendingPreviewMusic;
			pendingPreviewMusic = MusicIndex::None;
			BeatMediaSystem::Instance().FadeInMp3(activePreviewMusic, kSongPreviewVolume, kSongPreviewFadeSeconds, true);
			RefreshSelectionUi();
		}
		return;
	}

	if (activePreviewMusic == MusicIndex::None)
		return;

	const double length = BeatMediaSystem::Instance().GetMp3LengthSeconds(activePreviewMusic);
	const double position = BeatMediaSystem::Instance().GetMp3PositionSeconds(activePreviewMusic);
	BeatMfPlayer* player = BeatMediaSystem::Instance().GetMp3(activePreviewMusic);
	const bool reachedEnd = length > 0.1 && position >= length - 0.05;
	if ((player != nullptr && player->IsPlaybackEnded()) || reachedEnd)
		BeatMediaSystem::Instance().PlayMp3(activePreviewMusic, true);
}

void RhythmGameManager::StopSongPreview()
{
	if (activePreviewMusic != MusicIndex::None)
		BeatMediaSystem::Instance().FadeOutMp3(activePreviewMusic, kSongPreviewFadeSeconds, true);

	if (pendingPreviewMusic != MusicIndex::None)
		pendingPreviewMusic = MusicIndex::None;

	previewStaticTimerSeconds = 0.0f;
	activePreviewMusic = MusicIndex::None;
	BeatMediaSystem::Instance().StopSfx(SFXIndex::televisionStatic);
}
void RhythmGameManager::UpdateProgressBar(double songTime)
{
	if (progressFillObject == nullptr)
		return;

	const double length = std::max(1.0, cachedSongLength);
	const float progress = static_cast<float>(std::clamp(songTime / length, 0.0, 1.0));
	const float fullWidth = progressFillBaseScale.x * 100.0f;
	const float currentWidth = fullWidth * progress;
	const float left = progressFillBasePosition.x - fullWidth * 0.5f;

	progressFillObject->transform()->SetScale(Vector2(progressFillBaseScale.x * std::max(0.01f, progress), progressFillBaseScale.y));
	progressFillObject->transform()->SetPosition(Vector2(left + currentWidth * 0.5f, progressFillBasePosition.y));

	if (progressFillSprite != nullptr)
		progressFillSprite->color = Color(0.25f, 0.85f, 1.0f, 1.0f);
}

void RhythmGameManager::OnPlayerSkillGaugeChanged(void* context, int currentGauge, int maxGauge)
{
	RhythmGameManager* manager = static_cast<RhythmGameManager*>(context);
	if (manager != nullptr)
		manager->UpdateSkillGaugeView(currentGauge, maxGauge);
}

void RhythmGameManager::UpdateSkillGaugeView(int currentGauge, int maxGauge)
{
	if (skillGaugeFillObject == nullptr)
		return;

	const float ratio = maxGauge <= 0 ? 0.0f : ClampFloat(static_cast<float>(currentGauge) / static_cast<float>(maxGauge), 0.0f, 1.0f);
	const float fullWidth = skillGaugeFillBaseScale.x * 100.0f;
	const float currentWidth = fullWidth * ratio;
	const float left = skillGaugeFillBasePosition.x - fullWidth * 0.5f;

	skillGaugeFillObject->transform()->SetScale(Vector2(skillGaugeFillBaseScale.x * std::max(0.01f, ratio), skillGaugeFillBaseScale.y));
	skillGaugeFillObject->transform()->SetPosition(Vector2(left + currentWidth * 0.5f, skillGaugeFillBasePosition.y));

	if (skillGaugeFillSprite != nullptr)
	{
		skillGaugeFillSprite->color = ratio >= 1.0f
			? Color(1.0f, 0.88f, 0.20f, 1.0f)
			: Color(0.55f, 0.38f, 1.0f, 1.0f);
	}

	ToggleUltiCommand(IsPlaying() && ratio >= 1.0f);
}

void RhythmGameManager::ToggleUltiCommand(bool visible)
{
	if (ultiCommandObject == nullptr)
		return;

	if (visible && !ultiCommandVisible)
		ultiCommandTimerSeconds = 0.0f;

	ultiCommandVisible = visible;
	ultiCommandObject->SetActive(visible);

	if (!visible)
		ultiCommandTimerSeconds = 0.0f;
}

void RhythmGameManager::UpdateUltiCommand(float deltaSeconds)
{
	if (!ultiCommandVisible || ultiCommandSprite == nullptr)
		return;

	ultiCommandTimerSeconds += deltaSeconds;
	const float cycleTime = std::fmod(ultiCommandTimerSeconds, kUltiCommandAnimationSeconds);
	const float progress = cycleTime / kUltiCommandAnimationSeconds;
	const int frame = std::min(kUltiCommandFrameCount - 1, static_cast<int>(progress * kUltiCommandFrameCount));

	ultiCommandSprite->useAtlas = true;
	ultiCommandSprite->sprite = SpriteIndex::ultiCommand;
	ultiCommandSprite->currentAtlas = SpriteAtlasRect(0, frame, kUltiCommandFrameWidth, kUltiCommandFrameHeight);
	ultiCommandSprite->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
}

void RhythmGameManager::ScheduleSpawns(double songTime)
{
	int guard = 0;
	while (songTime >= BeatToTime(static_cast<float>(nextBeat)) - kSpawnLookAhead && guard++ < 16)
	{
		if (cachedSongLength > 0.0 && BeatToTime(static_cast<float>(nextBeat)) >= cachedSongLength - kSongEndNoThreatSeconds)
			break;

		SpawnPhrase(static_cast<float>(nextBeat));
		nextBeat += spawnBeatStep;
	}
}

bool RhythmGameManager::IsSongEndCleanupTime(double songTime) const
{
	return cachedSongLength > kSongEndNoThreatSeconds && songTime >= cachedSongLength - kSongEndNoThreatSeconds;
}

void RhythmGameManager::ClearSongEndThreats()
{
	if (songEndCleanupDone)
		return;

	DeactivateAllEnemies();
	DeactivateAllGuardAttacks();
	attackWarningLights.clear();
	songEndCleanupDone = true;
}

void RhythmGameManager::SpawnOpeningPattern()
{
	//난이도 규칙을 첫 패턴부터 지킵니다.
	// Easy는 정박 일반 적만, Normal은 정박 가드 공격 추가, Hard는 여기에 엇박 적을 추가합니다.
	const RhythmDifficultyConfig& difficulty = GetRhythmDifficulty(selectedDifficultyIndex);

	SpawnEnemy(0.0f, RhythmDirection::Left, 2);
	SpawnEnemy(1.0f, RhythmDirection::Right, 2);
	enemyArrivalBeats.insert(BeatKey(2.0f));
	enemyArrivalBeats.insert(BeatKey(3.0f));

	if (difficulty.attackChanceScale > 0.0f)
	{
		SpawnGuardAttack(2.0f, RhythmDirection::Up);
		attackArrivalBeats.insert(BeatKey(4.0f));
	}

	if (difficulty.offbeatChance > 0.0f)
	{
		SpawnEnemy(1.5f, RhythmDirection::UpRight, 2);
		enemyArrivalBeats.insert(BeatKey(3.5f));
	}
}

void RhythmGameManager::SpawnPhrase(float baseBeat)
{
	const float spacing = PickChainSpacing();
	const int chainCount = PickChainCount(spacing);
	const int travelBeats = PickTravelBeats();

	for (int i = 0; i < chainCount; ++i)
	{
		const float beat = baseBeat + i * spacing;
		if (cachedSongLength > 0.0 && BeatToTime(beat) >= cachedSongLength - kSongEndNoThreatSeconds)
			break;

		SpawnGroup(beat, travelBeats);
	}
}

void RhythmGameManager::SpawnGroup(float beat, int travelBeats)
{
	const std::vector<RhythmDirection> directions = PickSpawnPattern();
	const int spawnKind = PickSpawnKind(beat, travelBeats);
	if (spawnKind < 0)
		return;

	for (RhythmDirection direction : directions)
	{
		if (spawnKind == 1)
			SpawnGuardAttack(beat, direction);
		else
			SpawnEnemy(beat, direction, travelBeats);
	}
}

int RhythmGameManager::PickSpawnKind(float beat, int travelBeats)
{
	const RhythmSongConfig& song = GetRhythmSong(selectedSongIndex);
	const RhythmDifficultyConfig& difficulty = GetRhythmDifficulty(selectedDifficultyIndex);
	const float enemyArrivalBeat = beat + static_cast<float>(travelBeats);
	const float attackArrivalBeat = beat + 2.0f;
	const bool canSpawnEnemy = attackArrivalBeats.find(BeatKey(enemyArrivalBeat)) == attackArrivalBeats.end();
	const bool canSpawnAttack = enemyArrivalBeats.find(BeatKey(attackArrivalBeat)) == enemyArrivalBeats.end();
	const float attackChance = std::clamp(song.attackChance * difficulty.attackChanceScale, 0.0f, 1.85f);
	//Easy처럼 공격 배율이 0인 난이도에서는 빈 비트가 생기더라도 가드 공격을 강제로 만들지 않습니다.
	const bool attacksAllowed = difficulty.attackChanceScale > 0.0f;
	const bool useAttack = attacksAllowed && canSpawnAttack && (!canSpawnEnemy || RandomFloat(0.0f, 1.0f) < attackChance);

	if (useAttack)
	{
		attackArrivalBeats.insert(BeatKey(attackArrivalBeat));
		return 1;
	}

	if (canSpawnEnemy)
	{
		enemyArrivalBeats.insert(BeatKey(enemyArrivalBeat));
		return 0;
	}

	return -1;
}

void RhythmGameManager::SpawnEnemy(float beat, RhythmDirection direction, int travelBeats)
{
	RhythmEnemyController* enemy = FindFreeEnemy();
	if (enemy == nullptr)
		return;

	const Vector2 target = PlayerPosition();
	const Vector2 start = target + RhythmDirectionVector(direction) * RhythmSpawnDistanceForDirection(direction);
	const double spawnTime = BeatToTime(beat);
	const double hitTime = BeatToTime(beat + static_cast<float>(travelBeats));
	const RhythmSongConfig& song = GetRhythmSong(selectedSongIndex);

	enemy->Activate(
		direction,
		start,
		target,
		spawnTime,
		hitTime,
		HitWindowSeconds(),
		BeatSec(),
		song.movePortion,
		song.movePower,
		travelBeats,
		this);
}

void RhythmGameManager::SpawnGuardAttack(float beat, RhythmDirection direction)
{
	RhythmGuardAttackController* attack = FindFreeGuardAttack();
	if (attack == nullptr)
		return;

	const Vector2 target = PlayerPosition();
	const Vector2 start = target + RhythmDirectionVector(direction) * RhythmSpawnDistanceForDirection(direction);
	const double warningTime = BeatToTime(beat);
	const double launchTime = BeatToTime(beat + 1.0f);
	const double hitTime = BeatToTime(beat + 2.0f);

	attackWarningLights.push_back(LightCue{ warningTime, start, true });
	attack->Activate(direction, start, target, launchTime, hitTime, HitWindowSeconds(), this);
}

int RhythmGameManager::PickTravelBeats()
{
	const std::vector<int>& beats = GetRhythmSong(selectedSongIndex).travelBeats;
	if (beats.empty())
		return 3;

	const int index = RandomInt(0, static_cast<int>(beats.size()) - 1);
	return std::max(1, beats[index]);
}

float RhythmGameManager::PickChainSpacing()
{
	const RhythmDifficultyConfig& difficulty = GetRhythmDifficulty(selectedDifficultyIndex);
	if (RandomFloat(0.0f, 1.0f) < difficulty.offbeatChance)
		return std::clamp(difficulty.offbeatStep, 0.1f, 0.9f);

	return static_cast<float>(std::max(1, GetRhythmSong(selectedSongIndex).chainStep));
}

int RhythmGameManager::PickChainCount(float spacing)
{
	const RhythmSongConfig& song = GetRhythmSong(selectedSongIndex);
	const RhythmDifficultyConfig& difficulty = GetRhythmDifficulty(selectedDifficultyIndex);
	if (RandomFloat(0.0f, 1.0f) > song.chainChance * std::clamp(difficulty.chainScale, 0.0f, 1.0f))
		return 1;

	const float safeSpacing = std::max(0.1f, spacing);
	const int maxBeforeNextBase = std::max(1, static_cast<int>(std::floor((std::max(1, spawnBeatStep) - 0.01f) / safeSpacing)) + 1);
	const int maxCount = std::max(1, std::min(song.chainMax, maxBeforeNextBase));
	const int minCount = std::max(1, std::min(song.chainMin, maxCount));
	return RandomInt(minCount, maxCount);
}

std::vector<RhythmDirection> RhythmGameManager::PickSpawnPattern()
{
	static const std::array<std::vector<RhythmDirection>, 5> singlePatterns = {
		std::vector<RhythmDirection>{ RhythmDirection::Left },
		std::vector<RhythmDirection>{ RhythmDirection::Right },
		std::vector<RhythmDirection>{ RhythmDirection::Up },
		std::vector<RhythmDirection>{ RhythmDirection::UpLeft },
		std::vector<RhythmDirection>{ RhythmDirection::UpRight },
	};

	static const std::array<std::vector<RhythmDirection>, 6> multiPatterns = {
		std::vector<RhythmDirection>{ RhythmDirection::Left },
		std::vector<RhythmDirection>{ RhythmDirection::Right },
		std::vector<RhythmDirection>{ RhythmDirection::Up },
		std::vector<RhythmDirection>{ RhythmDirection::UpLeft },
		std::vector<RhythmDirection>{ RhythmDirection::UpRight },
		std::vector<RhythmDirection>{ RhythmDirection::Left, RhythmDirection::Right },
	};

	const RhythmDifficultyConfig& difficulty = GetRhythmDifficulty(selectedDifficultyIndex);
	if (difficulty.allowMultiPatterns)
		return multiPatterns[RandomInt(0, static_cast<int>(multiPatterns.size()) - 1)];

	return singlePatterns[RandomInt(0, static_cast<int>(singlePatterns.size()) - 1)];
}

RhythmEnemyController* RhythmGameManager::FindFreeEnemy()
{
	for (RhythmEnemyController* enemy : enemyPool)
	{
		if (enemy != nullptr && !enemy->IsActiveEnemy())
			return enemy;
	}

	return nullptr;
}

RhythmGuardAttackController* RhythmGameManager::FindFreeGuardAttack()
{
	for (RhythmGuardAttackController* attack : guardAttackPool)
	{
		if (attack != nullptr && !attack->IsActiveAttack())
			return attack;
	}

	return nullptr;
}

void RhythmGameManager::RegisterMiss()
{
	runStats.RecordMiss();

	if (playerController != nullptr)
	{
		BeatMediaSystem::Instance().PlaySfx(SFXIndex::aodThreeHitsMix, 0.65f, true);
		playerController->TakeDamage(MissDamage());
	}
}

void RhythmGameManager::RegisterWhiff()
{
	runStats.RecordWhiff(WhiffBreak());
}

void RhythmGameManager::TickEffects(float deltaSeconds)
{
	for (RhythmEffectController* effect : effectPool)
	{
		if (effect != nullptr)
			effect->Tick(deltaSeconds);
	}
}

void RhythmGameManager::ProcessAttackWarningLights(double songTime)
{
	for (int i = static_cast<int>(attackWarningLights.size()) - 1; i >= 0; --i)
	{

		if (songTime < attackWarningLights[i].time)
			continue;

		SpawnLight(SpriteIndex::blueLight, attackWarningLights[i].position, std::max(kLightTime, BeatSec() * 0.6f), kLightAlpha, kLightWidth);

		//이거 없으면 만들자 마자 지워짐
		if (songTime < attackWarningLights[i].time)
			continue;
		
		attackWarningLights.erase(attackWarningLights.begin() + i);
	}
}

void RhythmGameManager::StartSkillVideo(int skillIndex)
{
	static const std::array<VideoIndex, 3> skillVideos = {
		VideoIndex::Suna,
		VideoIndex::GungWoo,
		VideoIndex::Aria,
	};

	const int index = std::clamp(skillIndex, 0, static_cast<int>(skillVideos.size()) - 1);
	activeSkillVideo = skillVideos[index];
	activeSkillEffect = activeSkillVideo;
	skillEffectRemainingSeconds = 0.0f;
	if (activeSkillEffect == VideoIndex::Suna && playerController != nullptr)
		playerController->Heal(playerController->MaxHealth() / 2);
	else if (activeSkillEffect == VideoIndex::Aria || activeSkillEffect == VideoIndex::GungWoo)
		skillEffectRemainingSeconds = kSkillAutoClearSeconds;

	skillVideoTimerSeconds = 0.0f;
	skillVideoEndHoldSeconds = 0.0f;
	skillVideoFinishSfxPlayed = false;
	BeatMediaSystem::Instance().SetMp3Volume(GetRhythmSong(selectedSongIndex).musicId, kSkillVideoMusicVolume);
	BeatMediaSystem::Instance().StopAllMp4();
	skillVideoActive = BeatMediaSystem::Instance().PlayMp4(activeSkillVideo, true);
	if (!skillVideoActive)
		BeatMediaSystem::Instance().SetMp3Volume(GetRhythmSong(selectedSongIndex).musicId, kGameplayMusicVolume);

	float timeout = 0.05f;
	float elapsed = 0.0f;

	//임시
	//동일 프레임에서 동영상 플레이시간을 가져오지 못해서 0.05초 제한으로 반복 돌림
	while (skillVideoDurationSeconds == 0 && elapsed < timeout)
	{
		Sleep(10);
		elapsed += 0.01f;

		skillVideoDurationSeconds =
			(float)BeatMediaSystem::Instance().GetMp4LengthSeconds(activeSkillVideo);
	}
	if (skillVideoDurationSeconds <= 0.1f)
		skillVideoDurationSeconds = 3.0f;
}

void RhythmGameManager::UpdateSkillVideo(float deltaSeconds)
{
	if (!skillVideoActive)
		return;

	skillVideoTimerSeconds += deltaSeconds;
	BeatMediaSystem::Instance().UpdateMp4Video(activeSkillVideo);

	if (skillVideoFinishSfxPlayed)
	{
		skillVideoEndHoldSeconds += deltaSeconds;
		if (skillVideoEndHoldSeconds >= kSkillVideoEndFrameHoldSeconds)
			StopSkillVideo(false);
		return;
	}

	const bool endedByPlayback = BeatMediaSystem::Instance().IsMp4PlaybackEnded(activeSkillVideo);
	const bool endedByTimer = skillVideoTimerSeconds >= skillVideoDurationSeconds;
	if (!endedByPlayback && !endedByTimer)
		ResolveSkillTargets(true, true);

	if (endedByPlayback || endedByTimer)
	{
		const SFXIndex finishSfx = FinishSfxForVideo(activeSkillVideo);
		if (finishSfx != SFXIndex::None)
			BeatMediaSystem::Instance().PlaySfx(finishSfx, 0.9f, false);

		skillVideoFinishSfxPlayed = true;
		skillVideoEndHoldSeconds = 0.0f;
		ResolveSkillTargets(true, true);
	}
}

void RhythmGameManager::StopSkillVideo(bool playFinishSfx)
{
	if (skillVideoActive)
	{
		const SFXIndex finishSfx = playFinishSfx ? FinishSfxForVideo(activeSkillVideo) : SFXIndex::None;
		if (finishSfx != SFXIndex::None)
			BeatMediaSystem::Instance().PlaySfx(finishSfx, 0.9f, false);

		BeatMediaSystem::Instance().StopMp4(activeSkillVideo);
	}

	if (musicPlaybackStarted)
		BeatMediaSystem::Instance().SetMp3Volume(GetRhythmSong(selectedSongIndex).musicId, kGameplayMusicVolume);

	skillVideoActive = false;
	skillVideoFinishSfxPlayed = false;
	skillVideoTimerSeconds = 0.0f;
	skillVideoDurationSeconds = 0.0f;
	skillVideoEndHoldSeconds = 0.0f;
}

void RhythmGameManager::UpdateSkillEffect(float deltaSeconds)
{
	if (skillEffectRemainingSeconds <= 0.0f)
		return;

	if (activeSkillEffect == VideoIndex::Aria)
		ResolveSkillTargets(true, false);
	else if (activeSkillEffect == VideoIndex::GungWoo)
		ResolveSkillTargets(false, true);

	skillEffectRemainingSeconds -= deltaSeconds;
	if (skillEffectRemainingSeconds <= 0.0f)
	{
		skillEffectRemainingSeconds = 0.0f;
		activeSkillEffect = VideoIndex::None;
	}
}

void RhythmGameManager::ResolveSkillTargets(bool clearEnemies, bool clearGuardAttacks)
{
	if (!clearEnemies && !clearGuardAttacks)
		return;

	const double songTime = CurrentSongTime();

	if (clearEnemies)
	{
		for (RhythmEnemyController* enemy : enemyPool)
		{
			if (enemy == nullptr || !enemy->IsVulnerableEnemy())
				continue;

			runStats.RecordHit();
			HitFx(*enemy, songTime);
			enemy->Kill();
		}
	}

	if (clearGuardAttacks)
	{
		for (RhythmGuardAttackController* attack : guardAttackPool)
		{
			if (attack == nullptr || !attack->IsActiveAttack())
				continue;

			runStats.RecordBlock();
			BlockFx(attack->Position());
			attack->Block();
		}
	}
}

void RhythmGameManager::HitFx(RhythmEnemyController& enemy, double songTime)
{
	(void)songTime;
	const float beatBoost = std::abs(enemy.HitErrorSeconds(songTime)) <= HitWindowSeconds() * 0.55f ? 1.25f : 1.0f;
	const int capped = std::min(runStats.combo, std::max(0, kComboCap));
	const float comboBoost = 1.0f + capped * kComboBonus;
	const float power = beatBoost * comboBoost;

	BeatMediaSystem::Instance().PlaySfx(SFXIndex::hit, 0.75f, true);
	Shake(kShakeTime, kShakePower * power);
	Punch(kPunchTime, kPunchScale + (power - 1.0f) * 0.1f);
	SpawnImpact(enemy.Position(), Color(1.0f, 0.45f, 0.18f, 1.0f), power);
}

void RhythmGameManager::BlockFx(const Vector2& position)
{
	BeatMediaSystem::Instance().PlaySfx(SFXIndex::parry, 0.65f, true);
	Shake(kShakeTime * 0.75f, kShakePower * 0.7f);
	Punch(kPunchTime, kPunchScale);
	SpawnImpact(position, Color(0.2f, 0.75f, 1.0f, 1.0f), 0.9f);
}

void RhythmGameManager::ClearVisibleEnemies()
{
	for (RhythmEnemyController* enemy : enemyPool)
	{
		if (enemy == nullptr || !enemy->IsActiveEnemy())
			continue;

		SpawnImpact(enemy->Position(), Color(1.0f, 0.95f, 0.35f, 1.0f), 1.15f);
		enemy->Deactivate();
	}
}

void RhythmGameManager::ShowApproachLight(const Vector2& position, bool fast)
{
	SpawnLight(fast ? SpriteIndex::redLight : SpriteIndex::yellowLight, position, kLightTime, kLightAlpha, kLightWidth);
}

void RhythmGameManager::SpawnLight(SpriteIndex sprite, const Vector2& position, float duration, float alpha, float widthScale)
{
	RhythmEffectController* effect = FindFreeEffect();
	if (effect == nullptr)
		return;

	constexpr float lightPixelSize = 1560.0f;
	const Vector2 scale(
		(lightPixelSize * std::max(0.01f, widthScale)) / 1536.0f,
		(lightPixelSize * std::max(0.01f, widthScale)) / 1024.0f);

	BeatMediaSystem::Instance().PlaySfx(SFXIndex::alarm, 0.7f, false);
	effect->Activate(RhythmEffectType::Light, sprite, position, scale, Color(1.0f, 1.0f, 1.0f, std::clamp(alpha, 0.0f, 1.0f)), duration);
}

void RhythmGameManager::SpawnActionEffect(RhythmDirection direction, RhythmActionKind actionKind)
{
	RhythmEffectController* effect = FindFreeEffect();
	if (effect == nullptr)
		return;

	const SpriteIndex sprite = actionKind == RhythmActionKind::Guard ? SpriteIndex::block : SpriteIndex::attackRange;
	const Vector2 scale = actionKind == RhythmActionKind::Guard ? Vector2(0.38f, 0.38f) : Vector2(0.62f, 0.62f);
	const Color color = actionKind == RhythmActionKind::Guard
		? Color(1.0f, 1.0f, 1.0f, 0.90f)
		: Color(1.0f, 1.0f, 1.0f, 0.82f);

	effect->Activate(RhythmEffectType::Light, sprite, DirectionTargetPosition(direction), scale, color, kActionVisibleSeconds);
}

void RhythmGameManager::SpawnImpact(const Vector2& position, const Color& color, float strength)
{
	const int shardCount = static_cast<int>(std::round(ClampFloat(7.0f + (11.0f - 7.0f) * ClampFloat(strength - 1.0f, 0.0f, 1.0f), 7.0f, 11.0f)));
	for (int i = 0; i < shardCount; ++i)
	{
		RhythmEffectController* effect = FindFreeEffect();
		if (effect == nullptr)
			return;

		const float angle = RandomFloat(0.0f, kRhythmPi * 2.0f);
		const float speed = RandomFloat(120.0f, 220.0f) * strength;
		const Vector2 direction(std::cos(angle), std::sin(angle));
		const Color shardColor = LerpColor(Color(1.0f, 1.0f, 1.0f, 1.0f), color, 0.85f);
		const float size = RandomFloat(0.07f, 0.13f) * strength;

		effect->Activate(
			RhythmEffectType::Shard,
			SpriteIndex::Sqaure,
			position,
			Vector2(size, size),
			shardColor,
			RandomFloat(0.16f, 0.25f),
			direction * speed);
	}
}

RhythmEffectController* RhythmGameManager::FindFreeEffect()
{
	for (RhythmEffectController* effect : effectPool)
	{
		if (effect != nullptr && !effect->IsActiveEffect())
			return effect;
	}

	return nullptr;
}

void RhythmGameManager::Shake(float duration, float amplitude)
{
	shakeDuration = std::max(0.001f, duration);
	shakeAmplitude = shakeRemaining <= 0.0f ? amplitude : std::max(shakeAmplitude, amplitude);
	shakeRemaining = std::max(shakeRemaining, shakeDuration);
}

void RhythmGameManager::Punch(float duration, float scale)
{
	punchDuration = std::max(0.001f, duration);
	punchRemaining = punchDuration;
	punchScale = std::max(1.0f, scale);
}

void RhythmGameManager::UpdateCameraShake(float deltaSeconds)
{
	if (cameraObject == nullptr)
		return;

	if (shakeRemaining <= 0.0f)
	{
		cameraObject->transform()->SetPosition(cameraBasePosition);
		shakeAmplitude = 0.0f;
		return;
	}

	shakeRemaining -= deltaSeconds;
	const float fade = shakeDuration <= 0.0f ? 0.0f : ClampFloat(shakeRemaining / shakeDuration, 0.0f, 1.0f);
	const Vector2 shakeOffset(RandomFloat(-1.0f, 1.0f), RandomFloat(-1.0f, 1.0f));
	cameraObject->transform()->SetPosition(cameraBasePosition + shakeOffset * shakeAmplitude * fade);
}

void RhythmGameManager::UpdatePunch(float deltaSeconds)
{
	if (playerObject == nullptr)
		return;

	if (punchRemaining <= 0.0f)
	{
		playerObject->transform()->SetScale(playerBaseScale);
		return;
	}

	punchRemaining -= deltaSeconds;
	const float progress = punchDuration <= 0.0f ? 1.0f : 1.0f - ClampFloat(punchRemaining / punchDuration, 0.0f, 1.0f);
	const float punch = std::sin(progress * kRhythmPi);
	playerObject->transform()->SetScale(playerBaseScale * (1.0f + (punchScale - 1.0f) * punch));
}

void RhythmGameManager::CheckGameEnd(double songTime)
{
	if (playerController != nullptr && playerController->IsDead())
	{
		endedByDeath = true;
		ChangeToResult();
		return;
	}

	const RhythmSongConfig& song = GetRhythmSong(selectedSongIndex);
	const BeatMfPlayer* musicPlayer = BeatMediaSystem::Instance().GetMp3(song.musicId);
	const bool endedByPlayback = musicPlaybackStarted && musicPlayer != nullptr && musicPlayer->IsPlaybackEnded();
	const bool endedByClock = cachedSongLength > 0.0 && songTime >= cachedSongLength - 0.05;

	if (!endedByPlayback && !endedByClock)
	{
		if (statusTextObject != nullptr)
		{
			statusTextObject->transform()->SetPosition(kStatusTextPlayingPosition);
			statusTextObject->transform()->SetScale(kStatusTextPlayingScale);
		}

		std::wstringstream stream;
		stream << L"COMBO " << runStats.combo
			<< L"\nENEMY " << runStats.hits
			<< L"\nPARRY " << runStats.blocks;
		UpdateStatusText(stream.str());
		return;
	}

	if (ActiveEnemyCount() > 0 || ActiveGuardAttackCount() > 0)
	{
		DeactivateAllEnemies();
		DeactivateAllGuardAttacks();
		attackWarningLights.clear();
	}

	ChangeToResult();
}

void RhythmGameManager::UpdateStatusText(const std::wstring& message)
{
	if (statusText != nullptr)
		statusText->text = message;
}

void RhythmGameManager::UpdateResultText()
{
	if (statusTextObject != nullptr)
	{
		statusTextObject->transform()->SetPosition(kStatusTextDefaultPosition);
		statusTextObject->transform()->SetScale(kStatusTextDefaultScale);
	}

	std::wstringstream stream;
	stream << (endedByDeath ? L"FAILED" : L"RESULT")
		<< L" / Hit " << runStats.hits
		<< L" / Block " << runStats.blocks
		<< L" / Miss " << runStats.misses
		<< L" / Best Combo " << runStats.bestCombo
		<< L" / Skill Gauge " << (playerController ? playerController->SkillGauge() : 0)
		<< L"/" << (playerController ? playerController->MaxSkillGauge() : 0)
		<< L" / Space: title";

	UpdateStatusText(stream.str());

	if (resultText != nullptr)
		resultText->text = stream.str();

	WriteDebugLine(stream.str());
}

void RhythmGameManager::WriteDebugLine(const std::wstring& message)
{
	std::string narrowMessage;
	narrowMessage.reserve(message.size());

	for (wchar_t ch : message)
		narrowMessage.push_back(ch <= 0x7f ? static_cast<char>(ch) : '?');

	Debug::Log(narrowMessage);
}

int RhythmGameManager::ActiveEnemyCount() const
{
	int count = 0;

	for (RhythmEnemyController* enemy : enemyPool)
	{
		if (enemy != nullptr && enemy->IsActiveEnemy())
			++count;
	}

	return count;
}

int RhythmGameManager::ActiveGuardAttackCount() const
{
	int count = 0;

	for (RhythmGuardAttackController* attack : guardAttackPool)
	{
		if (attack != nullptr && attack->IsActiveAttack())
			++count;
	}

	return count;
}

Vector2 RhythmGameManager::PlayerPosition() const
{
	if (playerObject == nullptr)
		return Vector2(0.0f, -350.0f);

	return playerObject->transform()->position();
}

Vector2 RhythmGameManager::DirectionTargetPosition(RhythmDirection direction) const
{
	return PlayerPosition() + RhythmDirectionEffectOffset(direction);
}

double RhythmGameManager::SongNow()
{
	const RhythmSongConfig& song = GetRhythmSong(selectedSongIndex);
	const BeatMfPlayer* musicPlayer = BeatMediaSystem::Instance().GetMp3(song.musicId);

	if (!musicPlaybackStarted || musicPlayer == nullptr || !musicPlayer->IsOpened())
		return currentSongTime;

	double songTime = BeatMediaSystem::Instance().GetMp3PositionSeconds(song.musicId);

	while (stateTimerSeconds > 0.05f && songTime <= 0.0 && !musicPlayer->IsPlaybackEnded())
	{
		Sleep(10);
		songTime = BeatMediaSystem::Instance().GetMp3PositionSeconds(song.musicId);
	}

	while (currentSongTime > 0.0 && songTime <= 0.0 && !musicPlayer->IsPlaybackEnded())
	{
		Sleep(10);
		songTime = BeatMediaSystem::Instance().GetMp3PositionSeconds(song.musicId);
	}

	if (musicPlayer->IsPlaybackEnded() && cachedSongLength > 0.0)
		return cachedSongLength;

	return songTime;
}

double RhythmGameManager::CurrentSongLengthSeconds() const
{
	const RhythmSongConfig& song = GetRhythmSong(selectedSongIndex);
	const BeatMfPlayer* musicPlayer = BeatMediaSystem::Instance().GetMp3(song.musicId);

	if (musicPlayer == nullptr || !musicPlayer->IsOpened())
		return 0.0;

	double length = BeatMediaSystem::Instance().GetMp3LengthSeconds(song.musicId);
	while (length <= 0.1)
	{
		Sleep(10);
		length = BeatMediaSystem::Instance().GetMp3LengthSeconds(song.musicId);
	}

	return length;
}

double RhythmGameManager::BeatToTime(float beat) const
{
	const RhythmSongConfig& song = GetRhythmSong(selectedSongIndex);
	return song.offsetSeconds + beat * BeatSec();
}

int RhythmGameManager::BeatKey(float beat) const
{
	return static_cast<int>(std::round(beat * 1000.0f));
}

float RhythmGameManager::BeatSec() const
{
	return 60.0f / std::max(1.0f, GetRhythmSong(selectedSongIndex).bpm);
}

float RhythmGameManager::DeltaSeconds() const
{
	return std::clamp(Time::deltaTime() * 0.001f, 0.0f, 0.05f);
}

float RhythmGameManager::UnscaledDeltaSeconds() const
{
	return std::clamp(Time::unscaledDeltaTime() * 0.001f, 0.0f, 0.05f);
}

float RhythmGameManager::HitWindowSeconds() const
{
	return GetRhythmDifficulty(selectedDifficultyIndex).hitWindowSeconds;
}

int RhythmGameManager::MissDamage() const
{
	return GetRhythmDifficulty(selectedDifficultyIndex).missDamage;
}

int RhythmGameManager::WhiffBreak() const
{
	return GetRhythmDifficulty(selectedDifficultyIndex).whiffBreak;
}

float RhythmGameManager::RandomFloat(float minValue, float maxValue)
{
	std::uniform_real_distribution<float> distribution(minValue, maxValue);
	return distribution(enemyRandom);
}

int RhythmGameManager::RandomInt(int minInclusive, int maxInclusive)
{
	std::uniform_int_distribution<int> distribution(minInclusive, maxInclusive);
	return distribution(enemyRandom);
}
