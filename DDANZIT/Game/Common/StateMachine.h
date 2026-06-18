#pragma once

template <typename TOwner>
class IState
{
public:
	virtual ~IState() = default;

	// 상태에 처음 들어올 때 한 번 호출됩니다.
	// UI 표시, 변수 리셋, 음악 시작처럼 "진입 순간"에 필요한 일을 둡니다.
	virtual void Enter(TOwner& owner) = 0;

	// 다른 상태로 넘어가기 직전에 한 번 호출됩니다.
	// 재생 중인 음악 정지, 풀 오브젝트 비활성화 같은 정리 작업을 둡니다.
	virtual void Exit(TOwner& owner) = 0;
};

template <typename TOwner>
class StateMachine
{
public:
	// 현재 상태를 정리하고 다음 상태의 Enter를 호출합니다.
	// 같은 상태로 다시 바꾸려는 요청은 무시해서 불필요한 초기화를 막습니다.
	void ChangeState(IState<TOwner>* nextState, TOwner& owner)
	{
		if (currentState == nextState)
			return;

		if (currentState != nullptr)
			currentState->Exit(owner);

		currentState = nextState;

		if (currentState != nullptr)
			currentState->Enter(owner);
	}

	IState<TOwner>* CurrentState() const
	{
		return currentState;
	}

private:
	IState<TOwner>* currentState = nullptr;
};
