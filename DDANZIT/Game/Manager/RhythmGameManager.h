#pragma once

#include "Color.h"
#include "MonoBehavior.h"
#include "Game/Manager/RhythmGameStates.h"
#include "Game/Common/RhythmTypes.h"
#include "Game/Common/StateMachine.h"

#include <array>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

class GameObject;
class HealthBarObserver;
class PlayerController;
class RhythmEffectController;
class RhythmEnemyController;
class RhythmGuardAttackController;
class SpriteRenderer;
class Text;

//곡 선택/준비/플레이/결과 상태를 관리하는 중앙 매니저입니다.
// 선택 UI는 현재 프로젝트 것을 유지하고, Playing 내부 규칙은 수정완 프로젝트 흐름에 맞췄습니다.
class RhythmGameManager : public MonoBehavior
{
public:
	RhythmGameManager(GameObject* gameObject);

	void Start() override;
	void Update() override;

	void EnterSongSelect();
	void UpdateSongSelect();
	void EnterPrepare();
	void UpdatePrepare();
	void EnterPlaying();
	void UpdatePlaying();
	void ExitPlaying();
	void EnterResult();
	void UpdateResult();

	void ChangeToSongSelect();
	void ChangeToPrepare();
	void ChangeToPlaying();
	void ChangeToResult();

	bool IsPlaying();
	int CurrentCombo() const;
	double CurrentSongTime() const;
	void OnPlayerAction(RhythmDirection direction, RhythmActionKind actionKind, const Vector2& position);
	void OnPlayerWhiff();
	void OnPlayerSkill(int comboSnapshot, int skillIndex);
	void OnPlayerExitCommand();
	void OnEnemyHit(RhythmEnemyController& enemy, double songTime);
	void OnGuardBlocked(const Vector2& position);
	void OnPlayerMissed();
	void ShowApproachLightFromEnemy(const Vector2& position, bool fast);

private:
	friend class SongSelectState;
	friend class GamePrepareState;
	friend class GamePlayingState;
	friend class GameResultState;

	struct LightCue
	{
		double time = 0.0;
		Vector2 position = Vector2(0.0f, 0.0f);
		bool fast = false;
	};

	void InitializeSceneObjects();
	void InitializePlayerForRun();
	void CollectEnemyPool();
	void CollectGuardAttackPool();
	void CollectEffectPool();
	void CollectSelectionUi();
	void CollectGameplayUi();
	void DeactivateAllEnemies();
	void DeactivateAllGuardAttacks();
	void DeactivateAllEffects();
	void SetSelectionUiVisible(bool visible);
	void SetGameplayUiVisible(bool visible);
	void RefreshSelectionUi();
	void SetSongSelectionStage(bool difficultyStage);
	void RefreshGameplayMarkers();
	void UpdateProgressBar(double songTime);
	static void OnPlayerSkillGaugeChanged(void* context, int currentGauge, int maxGauge);
	void UpdateSkillGaugeView(int currentGauge, int maxGauge);
	void ToggleUltiCommand(bool visible);
	void UpdateUltiCommand(float deltaSeconds);
	void QueueSongPreview(int songIndex);
	void UpdateSongPreview(float deltaSeconds);
	void StopSongPreview();

	void ScheduleSpawns(double songTime);
	bool IsSongEndCleanupTime(double songTime) const;
	void ClearSongEndThreats();
	void SpawnOpeningPattern();
	void SpawnPhrase(float baseBeat);
	void SpawnGroup(float beat, int travelBeats);
	int PickSpawnKind(float beat, int travelBeats);
	void SpawnEnemy(float beat, RhythmDirection direction, int travelBeats);
	void SpawnGuardAttack(float beat, RhythmDirection direction);
	int PickTravelBeats();
	float PickChainSpacing();
	int PickChainCount(float spacing);
	std::vector<RhythmDirection> PickSpawnPattern();
	RhythmEnemyController* FindFreeEnemy();
	RhythmGuardAttackController* FindFreeGuardAttack();

	void RegisterMiss();
	void RegisterWhiff();

	void TickEffects(float deltaSeconds);
	void ProcessAttackWarningLights(double songTime);
	void StartSkillVideo(int skillIndex);
	void UpdateSkillVideo(float deltaSeconds);
	void UpdateSkillEffect(float deltaSeconds);
	void StopSkillVideo(bool playFinishSfx = false);
	void ResolveSkillTargets(bool clearEnemies, bool clearGuardAttacks);

	void HitFx(RhythmEnemyController& enemy, double songTime);
	void BlockFx(const Vector2& position);
	void ClearVisibleEnemies();
	void ShowApproachLight(const Vector2& position, bool fast);
	void SpawnLight(SpriteIndex sprite, const Vector2& position, float duration, float alpha, float widthScale);
	void SpawnActionEffect(RhythmDirection direction, RhythmActionKind actionKind);
	void SpawnImpact(const Vector2& position, const Color& color, float strength);
	RhythmEffectController* FindFreeEffect();
	void Shake(float duration, float amplitude);
	void Punch(float duration, float scale);
	void UpdateCameraShake(float deltaSeconds);
	void UpdatePunch(float deltaSeconds);

	void CheckGameEnd(double songTime);
	void UpdateStatusText(const std::wstring& message);
	void UpdateResultText();
	void WriteDebugLine(const std::wstring& message);
	int ActiveEnemyCount() const;
	int ActiveGuardAttackCount() const;

	Vector2 PlayerPosition() const;
	Vector2 DirectionTargetPosition(RhythmDirection direction) const;
	double SongNow();
	double CurrentSongLengthSeconds() const;
	double BeatToTime(float beat) const;
	int BeatKey(float beat) const;
	float BeatSec() const;
	float DeltaSeconds() const;
	float UnscaledDeltaSeconds() const;
	float HitWindowSeconds() const;
	int MissDamage() const;
	int WhiffBreak() const;
	float RandomFloat(float minValue, float maxValue);
	int RandomInt(int minInclusive, int maxInclusive);

	StateMachine<RhythmGameManager> stateMachine;
	SongSelectState songSelectState;
	GamePrepareState prepareState;
	GamePlayingState playingState;
	GameResultState resultState;

	GameObject* playerObject = nullptr;
	GameObject* cameraObject = nullptr;
	PlayerController* playerController = nullptr;
	GameObject* healthBarObject = nullptr;
	HealthBarObserver* healthBarObserver = nullptr;
	GameObject* statusTextObject = nullptr;
	GameObject* resultTextObject = nullptr;
	Text* statusText = nullptr;
	Text* resultText = nullptr;
	Vector2 playerBaseScale = Vector2(1.3f, 1.3f);
	Vector2 cameraBasePosition = Vector2(0.0f, 0.0f);

	GameObject* menuStaticObject = nullptr;
	GameObject* menuSongPanelObject = nullptr;
	SpriteRenderer* menuSongPanelSprite = nullptr;
	GameObject* selectPanelObject = nullptr;
	GameObject* startHintObject = nullptr;
	GameObject* startHintLabelObject = nullptr;
	GameObject* songTitleObject = nullptr;
	std::array<GameObject*, kRhythmSongCount> songLabelObjects = {};
	GameObject* difficultyTitleObject = nullptr;
	std::array<GameObject*, 3> difficultyLabelObjects = {};
	std::vector<GameObject*> selectionExtraObjects;
	std::array<GameObject*, kRhythmSongCount> songCardObjects = {};
	std::array<SpriteRenderer*, kRhythmSongCount> songCardSprites = {};
	std::array<GameObject*, 3> difficultyCardObjects = {};
	std::array<SpriteRenderer*, 3> difficultyCardSprites = {};

	GameObject* progressTrackObject = nullptr;
	GameObject* progressFillObject = nullptr;
	SpriteRenderer* progressFillSprite = nullptr;
	GameObject* gameBackgroundObject = nullptr;
	Vector2 progressFillBasePosition = Vector2(0.0f, 0.0f);
	Vector2 progressFillBaseScale = Vector2(1.0f, 1.0f);
	GameObject* skillGaugeTrackObject = nullptr;
	GameObject* skillGaugeFillObject = nullptr;
	SpriteRenderer* skillGaugeFillSprite = nullptr;
	GameObject* ultiCommandObject = nullptr;
	SpriteRenderer* ultiCommandSprite = nullptr;
	Vector2 skillGaugeFillBasePosition = Vector2(0.0f, 0.0f);
	Vector2 skillGaugeFillBaseScale = Vector2(1.0f, 1.0f);
	float ultiCommandTimerSeconds = 0.0f;
	bool ultiCommandVisible = false;
	std::array<GameObject*, 5> hitTargetObjects = {};
	std::array<SpriteRenderer*, 5> hitTargetSprites = {};

	RhythmRunStats runStats;
	int selectedSongIndex = 0;
	int selectedDifficultyIndex = 1;
	bool difficultySelectActive = false;
	MusicIndex activePreviewMusic = MusicIndex::None;
	MusicIndex pendingPreviewMusic = MusicIndex::None;
	float previewStaticTimerSeconds = 0.0f;
	float stateTimerSeconds = 0.0f;
	float gameSeconds = 0.0f;
	double currentSongTime = 0.0;
	double cachedSongLength = 60.0;
	int nextBeat = 0;
	int spawnBeatStep = 1;
	bool sceneInitialized = false;
	bool endedByDeath = false;
	bool musicPlaybackStarted = false;
	bool songEndCleanupDone = false;
	bool skillVideoActive = false;
	bool skillVideoFinishSfxPlayed = false;
	float skillVideoTimerSeconds = 0.0f;
	float skillVideoDurationSeconds = 0.0f;
	float skillVideoEndHoldSeconds = 0.0f;
	VideoIndex activeSkillVideo = VideoIndex::Aria;
	VideoIndex activeSkillEffect = VideoIndex::None;
	float skillEffectRemainingSeconds = 0.0f;
	std::mt19937 enemyRandom;

	std::vector<RhythmEnemyController*> enemyPool;
	std::vector<RhythmGuardAttackController*> guardAttackPool;
	std::vector<RhythmEffectController*> effectPool;
	std::vector<LightCue> attackWarningLights;
	std::unordered_set<int> enemyArrivalBeats;
	std::unordered_set<int> attackArrivalBeats;

	float shakeDuration = 0.0f;
	float shakeRemaining = 0.0f;
	float shakeAmplitude = 0.0f;
	float punchDuration = 0.0f;
	float punchRemaining = 0.0f;
	float punchScale = 1.0f;
};
