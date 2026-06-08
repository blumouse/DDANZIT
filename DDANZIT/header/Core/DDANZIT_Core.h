#pragma once

#include <vector>
#include <queue>
#include "INC_Windows.h"

#include "DefineOption.h"

class GameObject;
class MonoBehavior;
class IDrawable;
class IRenderer;

// 임시로 여따 둡시다


struct DrawCommand
{
	float posX;
	float posY;
	float scaleX;
	float scaleY;
	float dirX;
	float dirY;
	SpriteIndex spriteIndex;
	int colorRGBA;
	bool flipX;
	bool flipY;
	bool useAtlas;
	int sliceWidth;
	int sliceHeight;
	int sliceIndexX;
	int sliceIndexY;
};

#ifdef USE_DEBUG

struct DebugDrawCommand
{
	float posX;
	float posY;
	float scaleX;
	float scaleY;
	int debugDrawType;
	int colorRGBA;
};

#endif // USE_DEBUG


namespace renderHelp
{
	class BitmapInfo;
}


// 엔진 내부 공개를 위한 클래스..였는데 음 몰라
class DDANZIT_Core 
{
	using BitmapInfo = renderHelp::BitmapInfo;

public:
	friend class GameObject;
	friend class MonoBehavior;
	friend class Camera;

#ifdef PROPS_MODE_2D

	friend class Draw2D;

#endif // PROPS_MODE_2D

	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize);
	friend void DDANZIT_Run();
	friend void DDANZIT_Finalize();

	friend void _OnResize(int width, int height);
	friend void _OnClose();
	
#pragma region Properties

private:
	HWND hWnd;
	static int width;
	static int height;

#ifdef RENDER_MODE_WINGDI

	HDC hFrontDC = nullptr;
	HDC hBackDC = nullptr;
	HBITMAP hBackBitmap = nullptr;
	HBITMAP hDefaultBitmap = nullptr;

	static std::vector<BitmapInfo*> bitmapResourceList;
	// TODO: 소리파일용 리소스도 확보

#endif // RENDER_MODE_WINGDI


#pragma endregion



#pragma region Methods

private:
	void InitGraphicSettings(HWND hWnd);
	void FinalizeGraphicSettings();

	void _OnResize(int width, int height);
	void _OnClose();

public:
	static int LoadBitmapResource(const wchar_t* filePath);

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

	// TODO_LATER: Object로 바꾸기?
	static std::queue<GameObject*> destroyScheduledQueue;

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
	static std::vector<IDrawable*> drawableList;
	static std::vector<DrawCommand> drawCommandLists[MAX_LAYER_NUM];	// 배칭을 어케 할수잇을까

#ifdef USE_DEBUG

	static std::vector<DebugDrawCommand> debugDrawCommandLists[MAX_LAYER_NUM];

#endif // USE_DEBUG

	static void RegisterDrawable(IDrawable* drawable);
	static void QuitDrawable(IDrawable* drawable);


private:
	void _InitDraw();
	void _Sketch();
	void _Render();
	//void _PostProcess();
	void _Present();
	void _Clear();

#pragma endregion



	// TODO: 이벤트로 가자 function 인풋매니저를 여기 하나또 둘까
#pragma region InputEvents

	void OnMouseMove(int x, int y);
	void OnLButtonDown(int x, int y);
	void OnLButtonUp(int x, int y);
	void OnRButtonDown(int x, int y);
	void OnRButtonUp(int x, int y);

#pragma endregion

};