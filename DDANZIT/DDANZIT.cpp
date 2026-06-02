#include "DDANZIT.h"
#include "DefineOption.h"

#include "GameTimer.h"

#include "DDANZIT_Core.h"

#include "Scene.h"

#include "IDrawable.h"
#include "GameObject.h"

#include "Lifecycle.h"
#include "Transform.h"

#include "INC_Windows.h"
#include "RenderHelp.h"

#include <vector>
#include <queue>

using namespace std;


// 지금 하고있는것...
// 하이라키 타입 만들기 씬에서 관리
// 라이프사이클 큐 관리 실행까지 이거 일단 걍 씬에서 갖는거로 하자 -> 액티브계열 함수 호출등록해줘야돼


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
// 스마트포인터 적용하기..? (차차)
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


    // 내부로직에서 접근가능하게 빼기
    static DDANZIT_Core gameCore;


    // TODO: 이것도 리스트로 바꿔?
    BitmapInfo* ppBitmapResources[MAX_RESOURCE_NUM];
    int bmiIndex = 0;

    bool PreLoadResources(const wchar_t** pfilePath, unsigned int size);



    // Pipeline
    // 걍 때려박음 (접근 이슈)
    //void DDANZIT_Update();
    //void DDANZIT_Render();


    // Lifecycles
    // 코어로 이사감
    //void _Awake();
    //void _OnEnable();
    //void _Start();

    //void _Update();
    //void _FixedUpdate();
    //void _LateUpdate();

    //void _OnDisable();
    //void _OnDestroy();


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



    return true;
}

bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize)
{
    if (!PreLoadResources(pfilePath, resourceSize))
        return false;

    return DDANZIT_Initialize(windowName, width, height);
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
            // 프레임 시작!
            // 유니티 라이프사이클 순서를 따름

            gameCore._Awake();

            gameCore._OnEnable();

            gameCore._Start();


            /* DDANZIT_Update() */
            {
                // TODO: Time 클래스에 접근해서 틱
                pGameTimer->Tick();


                while (fFrameCount >= 200.0f)
                {
                    gameCore._FixedUpdate();

                    // gameCore._OnTrigger...();
                    // gameCore._OnCollision...();

                    // gameCore._WaitForFixedUpdate();

                    fFrameCount -= 200.0f;
                }


                // gameCore._OnMouse...();


                gameCore._Update();

                // gameCore._WaitForSeconds();
                // gameCore._StartCoroutine();

                gameCore._LateUpdate();


                fDeltaTime = pGameTimer->DeltaTimeMS();
                fFrameCount += fDeltaTime;
            }


            /* DDANZIT_Render() */
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


            // gameCore._WaitForEndOfFrame();


            gameCore._OnDisable();

            gameCore._OnDestroy();


            /* DestroyScheduled */
            {
                while (!gameCore.destroyScheduledQueue.empty())
                {
                    GameObject* gameObject = gameCore.destroyScheduledQueue.front();
                    Scene* targetScene = gameObject->_scene;


                    // 라이프사이클 함수들 리스트에서 빼주기
                    for (Component* comp : gameObject->pComponentList)
                    {
                        //if (Lifecycle* lc = dynamic_cast<Lifecycle*>(comp))   은 보장이 되니까 스킵
                        Lifecycle* lc = dynamic_cast<Lifecycle*>(comp);
                        
                        if (lc->activeUpdate)
                            targetScene->updateExecList.erase(remove(
                                targetScene->updateExecList.begin(),
                                targetScene->updateExecList.end(), lc),
                                targetScene->updateExecList.end());

                        if (lc->activeFixedUpdate)
                            targetScene->fixedUpdateExecList.erase(remove(
                                targetScene->fixedUpdateExecList.begin(),
                                targetScene->fixedUpdateExecList.end(), lc),
                                targetScene->fixedUpdateExecList.end());

                        if (lc->activeLateUpdate)
                            targetScene->lateUpdateExecList.erase(remove(
                                targetScene->lateUpdateExecList.begin(),
                                targetScene->lateUpdateExecList.end(), lc),
                                targetScene->lateUpdateExecList.end());
                    }


                    // 씬 / 하이라키(루트)에서 빼버리기
                    // ..하이라키를 아직 안만들었네; 상관은 없지만서도
                    if (gameObject->_transform->_parent == nullptr)
                    {
                        targetScene->pRootGameObjectList.erase(remove(
                            targetScene->pRootGameObjectList.begin(),
                            targetScene->pRootGameObjectList.end(), gameObject),
                            targetScene->pRootGameObjectList.end());
                    }
                    else
                    {
                        gameObject->_transform->_parent->RemoveChild(gameObject->_transform);	// 별로 좋은 코드는 아니군

                        gameObject->_transform->_parent = nullptr;
                    }


                    // TODO_LATER: (게임아닌)오브젝트를 받는다면... 컴포넌트인 경우의 처리(분기)


                    // 삭제!
                    delete gameCore.destroyScheduledQueue.front();
                    gameCore.destroyScheduledQueue.pop();
                }


                //if (Application.isQuit)
                //    break;


                // 다음 프레임...
            }
        }
    }
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
    if (bmiIndex == MAX_RESOURCE_NUM)
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
            if (bmiIndex == MAX_RESOURCE_NUM)
            {
                // 오류..
                return false;
            }

            ppBitmapResources[bmiIndex++] = renderHelp::CreateBitmapInfo(pfilePath[i]);
        }

        return true;
    }


    // TODO: 이벤트 등록된 함수 호출
    // 아냐 이것도 큐로해야되;
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
