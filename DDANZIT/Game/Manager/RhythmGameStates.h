#pragma once

#include "Game/Common/StateMachine.h"

class RhythmGameManager;

// 게임 매니저 상태는 전환 순간의 Enter/Exit만 담당합니다.
// 매 프레임 입력, 타이머, 스폰 처리는 RhythmGameManager::Update에서 현재 상태를 보고 직접 실행합니다.
class SongSelectState : public IState<RhythmGameManager>
{
public:
	void Enter(RhythmGameManager& owner) override;
	void Exit(RhythmGameManager& owner) override;
};

class GamePrepareState : public IState<RhythmGameManager>
{
public:
	void Enter(RhythmGameManager& owner) override;
	void Exit(RhythmGameManager& owner) override;
};

class GamePlayingState : public IState<RhythmGameManager>
{
public:
	void Enter(RhythmGameManager& owner) override;
	void Exit(RhythmGameManager& owner) override;
};

class GameResultState : public IState<RhythmGameManager>
{
public:
	void Enter(RhythmGameManager& owner) override;
	void Exit(RhythmGameManager& owner) override;
};
