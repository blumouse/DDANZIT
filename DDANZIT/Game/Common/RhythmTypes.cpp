#include "Game/Common/RhythmTypes.h"

#include <algorithm>

namespace
{
	Vector2 Normalized(float x, float y)
	{
		Vector2 result(x, y);
		result.Normalize();
		return result;
	}
}

void RhythmRunStats::Reset()
{
	hits = 0;
	blocks = 0;
	misses = 0;
	whiffs = 0;
	combo = 0;
	bestCombo = 0;
}

void RhythmRunStats::RecordHit()
{
	++hits;
	whiffs = 0;
	++combo;
	bestCombo = std::max(bestCombo, combo);
}

void RhythmRunStats::RecordBlock()
{
	++blocks;
	whiffs = 0;
	++combo;
	bestCombo = std::max(bestCombo, combo);
}

void RhythmRunStats::RecordMiss()
{
	++misses;
	BreakCombo();
}

void RhythmRunStats::RecordWhiff(int whiffBreak)
{
	++whiffs;

	if (whiffs >= std::max(1, whiffBreak))
		BreakCombo();
}

void RhythmRunStats::ClearWhiffs()
{
	whiffs = 0;
}

void RhythmRunStats::BreakCombo()
{
	combo = 0;
	whiffs = 0;
}

void RhythmRunStats::RestoreComboAtLeast(int comboSnapshot)
{
	combo = std::max(combo, comboSnapshot);
	whiffs = 0;
	bestCombo = std::max(bestCombo, combo);
}

bool RhythmRunStats::HasWhiffs() const
{
	return whiffs > 0;
}

const std::array<RhythmSongConfig, kRhythmSongCount>& RhythmSongs()
{
	//수정완 프로젝트의 SongLibrary 값을 현재 리소스 enum 순서에 맞춰 옮겼습니다.
	static const std::array<RhythmSongConfig, kRhythmSongCount> songs = {
		RhythmSongConfig{ MusicIndex::zzz, MusicIndex::zzzHighlight, SpriteIndex::zzzPanel, L"ReDreaming Agel", 156.0f, 0.0f, 2.f, { 2, 3 }, 0.22f, 1.65f, 0.4f, 2, 3, 1, 0.33f },
		RhythmSongConfig{ MusicIndex::kanden, MusicIndex::kandenHighlight, SpriteIndex::kandenPanel, L"Kanden", 103.0f, 0.0f, 1.5f, { 2, 3 }, 0.22f, 1.65f, 0.4f, 2, 3, 1, 0.33f },
		RhythmSongConfig{ MusicIndex::HoYoHoYo, MusicIndex::HoYoHoYoHighlight, SpriteIndex::HoYoHoYoPanel, L"HoYoHoYo", 160.0f, 0.0f, 1.5f, { 2, 3 }, 0.22f, 1.65f, 0.4f, 2, 3, 1, 0.33f },
		RhythmSongConfig{ MusicIndex::PixelGalaxy, MusicIndex::PixelGalaxyHighlight, SpriteIndex::PixelGalaxyPanel, L"PixelGalaxy", 150.0f, 0.0f, 2.0f, { 2, 3 }, 0.22f, 1.65f, 0.4f, 2, 3, 1, 0.33f },
		RhythmSongConfig{ MusicIndex::SABMG, MusicIndex::SABMGHighlight, SpriteIndex::SABMGPanel, L"SABMG", 200.0f, 0.0f, 2.f, { 2, 3 }, 0.22f, 1.65f, 0.4f, 2, 3, 1, 0.33f },
		RhythmSongConfig{ MusicIndex::TripleCounter, MusicIndex::TripleCounterHighlight, SpriteIndex::TripleCounterPanel, L"TripleCounter", 162.0f, 0.0f, 1.0f, { 2, 3 }, 0.22f, 1.65f, 0.4f, 2, 3, 1, 0.33f },
		RhythmSongConfig{ MusicIndex::YoruNoOdoriko, MusicIndex::YoruNoOdorikoHighlight, SpriteIndex::YoruNoOdorikoPanel, L"\u591C\u306E\u8E0A\u308A\u5B50", 139.0f, 0.0f, 1.0f, { 2, 3 }, 0.22f, 1.65f, 0.4f, 2, 3, 1, 0.33f },
	};

	return songs;
}

const RhythmSongConfig& GetRhythmSong(int index)
{
	const std::array<RhythmSongConfig, kRhythmSongCount>& songs = RhythmSongs();
	int count = static_cast<int>(songs.size());

	while (index < 0)
		index += count;
	while (index >= count)
		index -= count;

	return songs[index];
}

int GetRhythmSongCount()
{
	return static_cast<int>(RhythmSongs().size());
}

const std::array<RhythmDifficultyConfig, 3>& RhythmDifficulties()
{
	//곡 선택 UI는 현재 것을 유지하되, 난이도는 수정완 플레이 규칙의 밀도/판정 보정으로 사용합니다.
	static const std::array<RhythmDifficultyConfig, 3> difficulties = {
		//Easy는 정박 일반 적만 나오도록 공격/엇박/복합 패턴을 막습니다.
		RhythmDifficultyConfig{ RhythmDifficulty::Easy, L"Easy", 0.28f, 8, 0.0f, 0.0f, 0.5f, 0.0f, false, 3 },
		//Normal은 정박을 유지하면서 가드 공격이 섞입니다.
		RhythmDifficultyConfig{ RhythmDifficulty::Normal, L"Normal", 0.22f, 12, 0.65f, 0.0f, 0.5f, 1.0f, false, 2 },
		//Hard는 Normal 규칙에 엇박과 복합 입력 패턴을 추가합니다.
		RhythmDifficultyConfig{ RhythmDifficulty::Hard, L"Hard", 0.16f, 18, 1.0f, 0.25f, 0.5f, 1.25f, true, 1 },
	};

	return difficulties;
}

const RhythmDifficultyConfig& GetRhythmDifficulty(int index)
{
	const std::array<RhythmDifficultyConfig, 3>& difficulties = RhythmDifficulties();
	int count = static_cast<int>(difficulties.size());

	while (index < 0)
		index += count;
	while (index >= count)
		index -= count;

	return difficulties[index];
}

int GetRhythmDifficultyCount()
{
	return static_cast<int>(RhythmDifficulties().size());
}

Vector2 RhythmDirectionVector(RhythmDirection direction)
{
	switch (direction)
	{
	case RhythmDirection::Left:
		return Vector2(-1.0f, 0.0f);
	case RhythmDirection::Right:
		return Vector2(1.0f, 0.0f);
	case RhythmDirection::Up:
		return Vector2(0.0f, 1.0f);
	case RhythmDirection::UpLeft:
		return Normalized(-1.0f, 1.0f);
	case RhythmDirection::UpRight:
		return Normalized(1.0f, 1.0f);
	default:
		return Vector2(0.0f, 0.0f);
	}
}

Vector2 RhythmDirectionEffectOffset(RhythmDirection direction)
{
	switch (direction)
	{
	case RhythmDirection::Left:
		return Vector2(-100.0f, 0.0f);
	case RhythmDirection::Right:
		return Vector2(100.0f, 0.0f);
	case RhythmDirection::Up:
		return Vector2(0.0f, 100.0f);
	case RhythmDirection::UpLeft:
		return Vector2(-90.0f, 90.0f);
	case RhythmDirection::UpRight:
		return Vector2(90.0f, 90.0f);
	default:
		return Vector2(0.0f, 0.0f);
	}
}

float RhythmSpawnDistanceForDirection(RhythmDirection direction)
{
	switch (direction)
	{
	case RhythmDirection::Left:
	case RhythmDirection::Right:
		return 750.0f;
	case RhythmDirection::Up:
		return 640.0f;
	case RhythmDirection::UpLeft:
	case RhythmDirection::UpRight:
		return 900.0f;
	default:
		return 750.0f;
	}
}

bool RhythmShouldFlipForDirection(RhythmDirection direction)
{
	return direction == RhythmDirection::Left || direction == RhythmDirection::UpLeft;
}

const char* RhythmDirectionName(RhythmDirection direction)
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
