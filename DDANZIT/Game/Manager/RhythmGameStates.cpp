#include "Game/Manager/RhythmGameStates.h"

#include "Game/Manager/RhythmGameManager.h"

void SongSelectState::Enter(RhythmGameManager& owner)
{
	owner.EnterSongSelect();
}

void SongSelectState::Exit(RhythmGameManager& owner)
{
}

void GamePrepareState::Enter(RhythmGameManager& owner)
{
	owner.EnterPrepare();
}

void GamePrepareState::Exit(RhythmGameManager& owner)
{
}

void GamePlayingState::Enter(RhythmGameManager& owner)
{
	owner.EnterPlaying();
}

void GamePlayingState::Exit(RhythmGameManager& owner)
{
	owner.ExitPlaying();
}

void GameResultState::Enter(RhythmGameManager& owner)
{
	owner.EnterResult();
}

void GameResultState::Exit(RhythmGameManager& owner)
{
}
