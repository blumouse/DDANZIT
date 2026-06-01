#include "DDANZIT.h"
#include "DefineOption.h"

#include "GameTimer.h"
#include "Scene.h"
#include "MyGameObject.h"
#include "IDrawable.h"
#include "Transform.h"

#include "INC_Windows.h"
#include "RenderHelp.h"

#include <vector>
#include <queue>

using namespace std;


//
// TODO 소기의 목표
// 
// 일단 이식하기! 제네릭 자료구조 적극사용
// 이름들 똑같이 바꾸기
// 전역으로 바꿀것들 바꾸기
// 추상화/옵션 지점들 - 렌더러, 디버거, 2d/3d - 등 고려하기
// 원본코드에(유틸) 있던것들 필요한거 갈무리 정리 쳐내기
// 오류 정보들 필요한거 던져주기
// 
// 추가할거..
// 씬 / 하이라키 구조 (사실 이 아래 씬으로 들어갈게 꽤 많은)
// 더 많은 오브젝트 / 컴포넌트 속성 (최소한 액티브, 이름, find동작)
// 코루틴 사이클
//


// 여기에 내부함수를 두면 된다
// 선언부
namespace
{
    // TODO: Time 네임스페이스 / 클래스로 분리할 필요가 있다
    GameTimer* pGameTimer = nullptr;
    float fDeltaTime = 0.0f;
    float fFrameCount = 0.0f;


    HWND g_hWnd = HWND();
    unsigned int g_width = 0;
    unsigned int g_height = 0;

    HDC hFrontDC = nullptr;
    HDC hBackDC = nullptr;
    HBITMAP hBackBitmap = nullptr;
    HBITMAP hDefaultBitmap = nullptr;


    vector<Scene*> pSceneList;


    // TODO: 벡터든 큐든 뭐.. 바꾸기.. 이전에 씬 / 하이라키로 넣기

    GameObjectBase** ppGameObjects = nullptr;
    int gameObjectsIndex = 0;

    IDrawable** ppDrawableLayers[MAX_LAYER_NUM];


    BitmapInfo* ppBitmapResources[MAX_BMI_NUM];
    int bmiIndex = 0;

    bool PreLoadResources(const wchar_t** pfilePath, unsigned int size);



    // Pipeline
    void _Update();
    void _Render();


    // Lifecycles
    void Awake();
    void Start();
    void Update();
    void FixedUpdate();
    void OnDestroy();


    // Input Events ...추가 예정
    void _OnMouseMove(int x, int y);
    void _OnLButtonDown(int x, int y);
    void _OnLButtonUp(int x, int y);
    void _OnRButtonDown(int x, int y);
    void _OnRButtonUp(int x, int y);


    void _OnResize(int width, int height);
    void _OnClose();
}


// 파라미터로 뭘 받아야할까 설정값들
// 창 이름 창 크기*2 리소스(초기) 정보

bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height) {

    pGameTimer = new GameTimer();
    pGameTimer->Reset();

    const wchar_t* className = L"DDANZIT";

    if (!Create(className, windowName, width, height))
        return false;


    RECT rcClient = {};
    GetClientRect(g_hWnd, &rcClient);
    width = rcClient.right - rcClient.left;
    height = rcClient.bottom - rcClient.top;

    hFrontDC = GetDC(g_hWnd);
    hBackDC = CreateCompatibleDC(hFrontDC);
    hBackBitmap = CreateCompatibleBitmap(hFrontDC, width, height);

    hDefaultBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);


    // 게임 초기화
    ppGameObjects = new GameObjectBase * [MAX_GAME_OBJECT_NUM];

    for (int i = 0; i < MAX_GAME_OBJECT_NUM; ++i)
    {
        ppGameObjects[i] = nullptr;
    }

    gameObjectsIndex = 0;

    for (int i = 0; i < MAX_LAYER_NUM; i++)
    {
        ppDrawableLayers[i] = new IDrawable * [MAX_GAME_OBJECT_NUM];

        for (int j = 0; j < MAX_GAME_OBJECT_NUM; j++)
            ppDrawableLayers[i][j] = nullptr;
    }


    return true;
}

void DDANZIT_Run() {

    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_MOUSEMOVE)
            {
                _OnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else if (msg.message == WM_LBUTTONDOWN)
            {
                _OnLButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else if (msg.message == WM_RBUTTONDOWN)
            {
                _OnRButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else if (msg.message == WM_LBUTTONUP)
            {
                _OnLButtonUp(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else if (msg.message == WM_RBUTTONUP)
            {
                _OnRButtonUp(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            _Update();
            _Render();
        }
    }
}

// 오버로딩
static bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize) 
{
    if (!PreLoadResources(pfilePath, resourceSize))
        return false;

    return DDANZIT_Initialize(windowName, width, height);
}


void DDANZIT_Finalize() {

    delete pGameTimer;
    pGameTimer = nullptr;

    for (int i = 0; i < MAX_LAYER_NUM; i++)
    {
        if (ppDrawableLayers[i])
        {
            delete ppDrawableLayers[i];
            ppDrawableLayers[i] = nullptr;
        }
    }

    if (ppGameObjects)
    {
        for (int i = 0; i < gameObjectsIndex; ++i)
        {
            if (ppGameObjects[i])
            {
                ppGameObjects[i]->OnDestroy();

                delete ppGameObjects[i];
                ppGameObjects[i] = nullptr;
            }
        }
        delete ppGameObjects;
        ppGameObjects = nullptr;
    }

    DestroyWnd();
}


BitmapInfo* LoadResource(const wchar_t* filePath)
{
    if (bmiIndex == MAX_BMI_NUM)
        return nullptr;

    ppBitmapResources[bmiIndex++] = renderHelp::CreateBitmapInfo(filePath);

    return ppBitmapResources[bmiIndex];
}


//GameObjectBase* GetObjectWithPos(int mouseX, int mouseY)
//{
//    GameObjectBase* gameObject = nullptr;
//
//    for (int i = 0; i < gameObjectsIndex; i++)
//    {
//        if (ppTransforms[i] && ppTransforms[i]->IsIntersectPoint(mouseX, mouseY))
//        {
//            gameObject = dynamic_cast<GameObjectBase*>(ppTransforms[i]);
//            break;
//        }
//    }
//
//    return gameObject;
//}
//
//bool TryGetObjectWithPos(int mouseX, int mouseY, GameObjectBase*& pGameObject)
//{
//    GameObjectBase* gameObject = nullptr;
//
//    for (int i = 0; i < gameObjectsIndex; i++)
//    {
//        if (ppTransforms[i] && ppTransforms[i]->IsIntersectPoint(mouseX, mouseY))
//        {
//            // 어 그 주소 go 맞아.. 더 확실하게 보장시킬수 없나 transform에?
//            if (gameObject = dynamic_cast<GameObjectBase*>(ppTransforms[i]))
//            {
//                pGameObject = gameObject;
//                return true;
//            }
//        }
//    }
//
//    return false;
//}



// 내부함수들..
// 구현부
namespace 
{
    bool PreLoadResources(const wchar_t** pfilePath, unsigned int size)
    {
        for (int i = 0; i < size; i++)
        {
            if (bmiIndex == MAX_BMI_NUM)
            {
                // 오류..
                return false;
            }

            ppBitmapResources[bmiIndex++] = renderHelp::CreateBitmapInfo(pfilePath[i]);
        }

        return true;
    }


    void _Update()
    {
        pGameTimer->Tick();

        Update();

        fDeltaTime = pGameTimer->DeltaTimeMS();
        fFrameCount += fDeltaTime;

        while (fFrameCount >= 200.0f)
        {
            FixedUpdate();
            fFrameCount -= 200.0f;
        }
    }

    // TODO: 배경색 스카이박스 설정 가능하게 (내부함수로)
    void _Render()
    {
        HBRUSH hGrayBrush = CreateSolidBrush(RGB(216, 216, 216));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hBackDC, hGrayBrush);

        ::PatBlt(hBackDC, 0, 0, g_width, g_height, PATCOPY);

        SelectObject(hBackDC, hOldBrush);
        DeleteObject(hGrayBrush);


        // 0번 레이어가 가장 위
        for (int i = MAX_LAYER_NUM - 1; i >= 0; i--)
        {
            for (int j = 0; j < gameObjectsIndex; ++j)
            {
                if (ppDrawableLayers[i][j])
                    ppDrawableLayers[i][j]->Draw(hBackDC);
            }
        }


        BitBlt(hFrontDC, 0, 0, g_width, g_height, hBackDC, 0, 0, SRCCOPY);
    }


    // TODO: 자료구조로 반영, 별도의 Awake / Start 큐에 등록 및 호출..
    void Awake() 
    {
        for (int i = 0; i < gameObjectsIndex; ++i)
        {
            if (ppGameObjects[i])
            {
                ppGameObjects[i]->Awake();
            }
        }
    }


    void Start()
    {
        for (int i = 0; i < gameObjectsIndex; ++i)
        {
            if (ppGameObjects[i])
            {
                ppGameObjects[i]->Start();
            }
        }
    }


    void Update()
    {
        for (int i = 0; i < gameObjectsIndex; ++i)
        {
            if (ppGameObjects[i])
            {
                ppGameObjects[i]->Update(fDeltaTime);
            }
        }
    }


    void FixedUpdate()
    {
        for (int i = 0; i < gameObjectsIndex; ++i)
        {
            if (ppGameObjects[i])
            {
                ppGameObjects[i]->FixedUpdate();
            }
        }
    }


    // TODO: 이벤트 등록된 함수 호출
    void _OnMouseMove(int x, int y)
    {
    }

    void _OnLButtonDown(int x, int y)
    {
    }

    void _OnRButtonDown(int x, int y)
    {
    }

    void _OnLButtonUp(int x, int y)
    {
    }

    void _OnRButtonUp(int x, int y)
    {
    }



    void _OnResize(int width, int height)
    {
        learning::SetScreenSize(width, height);

        OnResize(width, height);

        hBackBitmap = CreateCompatibleBitmap(hFrontDC, g_width, g_height);

        HANDLE hPrevBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);

        DeleteObject(hPrevBitmap);
    }


    void _OnClose()
    {
        SelectObject(hBackDC, hDefaultBitmap);

        DeleteObject(hBackBitmap);
        DeleteDC(hBackDC);

        ReleaseDC(g_hWnd, hFrontDC);
    }



    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {

        switch (msg)
        {
        case WM_SIZE:
        {
            GetWindowLongPtr(hwnd, GWLP_USERDATA);
            _OnResize(LOWORD(lparam), HIWORD(lparam));
        }

        case WM_CLOSE:
        {
            GetWindowLongPtr(hwnd, GWLP_USERDATA);
            _OnClose();
            PostQuitMessage(0);
            break;
        }


        default:
            return::DefWindowProc(hwnd, msg, wparam, lparam);
        }//switch

        return NULL;
    }


    bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height)
    {
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpszClassName = className;
        wc.lpfnWndProc = WndProc;


        ATOM classId = 0;
        if (!GetClassInfoEx(HINSTANCE(), className, &wc))
        {
            classId = RegisterClassEx(&wc);

            if (0 == classId) return false;
        }

        g_width = width;
        g_height = height;

        RECT rc = { 0, 0, width, height };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

        g_hWnd = CreateWindowEx(NULL, MAKEINTATOM(classId), L"",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
            rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), NULL);

        if (NULL == g_hWnd) return false;

        ::SetWindowText((HWND)g_hWnd, windowName);

        //SetWindowLongPtr((HWND)g_hWnd, GWLP_USERDATA, (LONG_PTR)this);

        ShowWindow((HWND)g_hWnd, SW_SHOW);
        UpdateWindow((HWND)g_hWnd);

        return true;
    }

    // 이름 바꿔야겠는데
    void DestroyWnd()
    {
        if (NULL != g_hWnd)
        {
            DestroyWindow((HWND)g_hWnd);
            g_hWnd = NULL;
        }
    }

    void OnResize(int width, int height)
    {
        g_width = width;
        g_height = height;
    }

}
