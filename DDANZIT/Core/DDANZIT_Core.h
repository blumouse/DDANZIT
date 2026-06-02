#pragma once

#include <vector>
#include <queue>

class Scene;
class GameObject;

namespace renderHelp
{
	class BitmapInfo;
}

// 엔진 내부 공개를 위한 클래스
// 정적으로 만드는게 낫나? 뭔가 좀 더러운 너낌스..
class DDANZIT_Core 
{
	using BitmapInfo = renderHelp::BitmapInfo;

public:
	friend class Scene;
	friend class GameObject;

	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend void DDANZIT_Run();
	friend void DDANZIT_Finalize();

#pragma region Constructor

#pragma endregion



#pragma region Properties

private:
	static Scene* mainScene;		// TODO: 이거 할당
	static std::vector<Scene*> pSceneList;
	static std::vector<Scene*> pLoadedSceneList;

	// TODO_LATER: Object로 바꾸기?
	static std::queue<GameObject*> destroyScheduledQueue;

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