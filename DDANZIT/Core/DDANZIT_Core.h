#pragma once

#include <vector>
#include <queue>

class Scene;
class GameObject;
class Lifecycle;
class MonoBehavior;

namespace renderHelp
{
	class BitmapInfo;
}


// 엔진 내부 공개를 위한 클래스..였는데
// 굳이 없어도 될것 같지만 보기좋으니 놔두자
class DDANZIT_Core 
{
	using BitmapInfo = renderHelp::BitmapInfo;

public:
	friend class GameObject;
	friend class MonoBehavior;

	friend class IDrawable;

	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend void DDANZIT_Run();
	friend void DDANZIT_Finalize();
	
#pragma region Properties

private:

	// TODO_LATER: Object로 바꾸기?
	std::queue<GameObject*> destroyScheduledQueue;

#pragma endregion



#pragma region Methods


#pragma endregion



#pragma region Lifecycles

private:
	void _Awake();
	void _OnEnable();
	void _Start();

	void _FixedUpdate();
	void _Update();
	void _LateUpdate();

	void _OnDisable();
	void _OnDestroy();

	// 컴포넌트 속성도 접근 가능하게, 모노비헤이비어로 받기
private:
	static std::queue<MonoBehavior*> awakeExecQueue;
	static std::queue<MonoBehavior*> onEnableExecQueue;
	static std::queue<MonoBehavior*> startExecQueue;

	static std::vector<MonoBehavior*> fixedUpdateExecList;
	static std::vector<MonoBehavior*> updateExecList;
	static std::vector<MonoBehavior*> lateUpdateExecList;

	static std::queue<MonoBehavior*> onDisableExecQueue;
	static std::queue<MonoBehavior*> onDestroyExecQueue;


	// 업데이트 훼손 방지용

	static std::queue<MonoBehavior*> registerUpdateScheduledQueue;
	static std::queue<MonoBehavior*> quitUpdateScheduledQueue;

	// 이건 위에서 호출

	void RegisterUpdateScheduled();
	void QuitUpdateScheduled();

	void DestroyScheduled();

	// 이건 아래에서 호출

	static void RegisterUpdateExecLists(MonoBehavior* behavior);
	static void QuitUpdateExecLists(MonoBehavior* behavior);

#pragma endregion



#pragma region Render

private:
	// TODO: 레이어(depth) 적용해서 만들기
	std::vector<IDrawable*> drawableRenderList;

	void RegisterDrawable(IDrawable* drawable);
	void RegisterDrawable(IDrawable* drawable, int layer);
	void QuitDrawable(IDrawable* drawable);
	void QuitDrawable(IDrawable* drawable, int layer);

#pragma endregion



	// TODO: 이벤트로 가자 function 인풋매니저를 여기 하나또 둘까
#pragma region InputEvents

	void OnMouseMove(int x, int y);
	void OnLButtonDown(int x, int y);
	void OnLButtonUp(int x, int y);
	void OnRButtonDown(int x, int y);
	void OnRButtonUp(int x, int y);

#pragma endregion

	BitmapInfo* LoadResource(const wchar_t* filePath);
};