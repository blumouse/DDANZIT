#pragma once

#include "DefineOption.h"
#include "Utillity.h"

#include <array>
#include <string>
#include <vector>

// 리듬 게임에서 공통으로 쓰는 방향, 난이도, 곡 설정, 플레이 기록 타입입니다.
// Manager/Player/Enemy가 같은 의미의 값을 공유해야 하므로 한 파일에 모았습니다.
enum class RhythmDirection
{
	Left,
	Right,
	Up,
	UpLeft,
	UpRight
};

//공격과 방어는 같은 방향 레인을 쓰지만, 입력 의미와 시각 효과가 다릅니다.
enum class RhythmActionKind
{
	Fire,
	Guard
};

// 스킬 입력용 방향입니다. 실제 커맨드는 Player.cpp의 SkillCommands()에서 관리합니다.
enum class RhythmSkillInput
{
	Left = 4,
	Down = 2,
	Right = 6,
	Up = 8
};

enum class RhythmDifficulty
{
	Easy,
	Normal,
	Hard
};

struct RhythmSongConfig
{
	MusicIndex musicId = MusicIndex::None;
	MusicIndex highlightMusicId = MusicIndex::None;
	SpriteIndex panelSprite = SpriteIndex::None;
	const wchar_t* title = L"";
	float bpm = 120.0f;
	float offsetSeconds = 0.0f;

	//수정완 버전처럼 곡마다 스폰 밀도와 이동감을 다르게 둡니다.
	float spawnEverySeconds = 2.0f;
	std::vector<int> travelBeats{ 2, 3 };
	float movePortion = 0.22f;
	float movePower = 1.65f;
	float chainChance = 0.4f;
	int chainMin = 2;
	int chainMax = 3;
	int chainStep = 1;
	float attackChance = 0.22f;
};

struct RhythmDifficultyConfig
{
	RhythmDifficulty difficulty = RhythmDifficulty::Normal;
	const wchar_t* title = L"";
	float hitWindowSeconds = 0.22f;
	int missDamage = 12;

	//난이도별로 수정완 버전의 체인/엇박/동시 패턴 허용 정도를 조절합니다.
	float chainScale = 0.0f;
	float offbeatChance = 0.0f;
	float offbeatStep = 0.5f;
	float attackChanceScale = 1.0f;
	bool allowMultiPatterns = false;
	int whiffBreak = 2;
};

struct RhythmRunStats
{
	int hits = 0;
	int blocks = 0;
	int misses = 0;
	int whiffs = 0;
	int combo = 0;
	int bestCombo = 0;

	void Reset();
	void RecordHit();
	void RecordBlock();
	void RecordMiss();
	void RecordWhiff(int whiffBreak);
	void ClearWhiffs();
	void BreakCombo();
	void RestoreComboAtLeast(int comboSnapshot);
	bool HasWhiffs() const;
};

constexpr int kRhythmSongCount = 7;

const std::array<RhythmSongConfig, kRhythmSongCount>& RhythmSongs();
const RhythmSongConfig& GetRhythmSong(int index);
int GetRhythmSongCount();

const std::array<RhythmDifficultyConfig, 3>& RhythmDifficulties();
const RhythmDifficultyConfig& GetRhythmDifficulty(int index);
int GetRhythmDifficultyCount();

Vector2 RhythmDirectionVector(RhythmDirection direction);
Vector2 RhythmDirectionEffectOffset(RhythmDirection direction);
float RhythmSpawnDistanceForDirection(RhythmDirection direction);
bool RhythmShouldFlipForDirection(RhythmDirection direction);
const char* RhythmDirectionName(RhythmDirection direction);
