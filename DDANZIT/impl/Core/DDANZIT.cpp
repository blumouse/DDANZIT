#include "DDANZIT.h"

#include "DefineOption.h"

#include "DDANZIT_Core.h"
#include "Application.h"
#include "SceneManager.h"

#include "GameTimer.h"
#include "INC_Windows.h"
#include "RenderHelp.h"
#include "Utillity.h"

#include <vector>
#include <queue>

using namespace std;


// 지금 하고있는것
// 
// 
// 다음은 바로 렌더러 구현 / 테스트하자.. <- 하는중
// 
// 아씨 트렌스폼 부모기준으로도 해야되는데..? 근데 이건 상위에서 결산할때 그렇게 하면 되기도 하고 <- 트랜스폼에 헬퍼 만들기
//
// 타임 아직도 안만들었어?
// 
// 배열들 미리 reserve해놓기?
// 
// GO에 달려있는 트랜스폼 이거 그냥 변수로 해도 될거같은데..(그게 나을듯) 근데 갖다쓴데가 많아서 엄청난 수정소요
// 걍 하지말자 이제와서..



//
// TODO 소기의 목표
// 
// 추가할거.. 순서
// 하이라키 구조  끗
// 씬 매니저    끗?
// 더 많은 오브젝트 / 컴포넌트 속성
// 코루틴 사이클
// 인풋시스템
// 애플리케이션
// 
// 
// 필드/메서드명 다시 정리하기...
// 접근자 뚫어논것도 좀 갈무리하기... (바뀐게 많아서)
// 원본코드에(유틸) 있던것들 필요한거 갈무리 정리 쳐내기
// 오류 정보들 필요한거 던져주기
// 킬체크 / 런타임 시에만 실행하도록 체크 (Run 이전 실행 금지)



// 얼추 한것들
//
// 
// 크앗 액티브 타이밍 이슈 다시해야돼 <- 된건가...?
// 파괴된 오브젝트 그 프레임동안 킬체크 접근불가하게 막아야함 <- 너무 많아.. 계획을 세워서 하자 아님걍 래핑;
// 그냥 함수마다 본인이 죽었으면 경고띄우면 되나? 일단 킵
// 
// 씬 매니저 만들기! <- 대강 되간다 로드 / 디스크립터는 너무갔어 일단 킵;
// 
// 하이라키 타입 만들기 씬에서 관리 <- 얼추 했는데 잘 되는진 모르겠어
// 
// 이거 add 씬 / 오브젝트 도 있어야겠는데..? 제네릭으로 받아야돼?
// 오브젝트 Add에서 복사생성자 인스턴트까지 <- 와 일단락! 남은건 테스트
//
// 간이로 디버그 레이어 일찍 만드는게 좋을지도 나도당장 디버그가 필요해.. <- 끝
// 
// 각도 - 벡터변환도 있어 <- 끝
// 
// 걍 빠르게 오브젝트 상속 주는게 나을지도 <- 안해
// 













// 시크릿 내부함수 선언부
namespace
{
    // TODO: Time 네임스페이스 / 클래스로 분리할 필요가 있다
    GameTimer* pGameTimer = nullptr;
    float fDeltaTime = 0.0f;
    float fFrameCount = 0.0f;


    HWND g_hWnd = HWND();
    unsigned int g_width = 0;
    unsigned int g_height = 0;


    // 내부로직에서 접근가능하게 빼기
    DDANZIT_Core gameCore;


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


    // 윈도우 관련

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height);
    void DestroyWnd();
    void OnResize(int width, int height);
}



// 파라미터로 뭘 받아야할까 설정값들
// 창 이름 창 크기*2 리소스(초기) 정보

bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height) {

    pGameTimer = new GameTimer();
    pGameTimer->Reset();

    const wchar_t* className = L"DDANZIT";

    if (!Create(className, windowName, width, height))
        return false;


    // 이쯤에서 그래픽 엔진 초기화
    gameCore.InitGraphicSettings(g_hWnd);


    Application::_isPlaying = false;
    Application::_isQuit = false;


    return true;
}

bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize)
{
    for (unsigned int i = 0; i < resourceSize; i++)
    {
        if (gameCore.bitmapResourceList.size() == MAX_RESOURCE_NUM)
        {
            // 오류..
            return false;
        }

        gameCore.bitmapResourceList.push_back(renderHelp::CreateBitmapInfo(pfilePath[i]));
    }

    return DDANZIT_Initialize(windowName, width, height);
}

void DDANZIT_Run() 
{
    Application::_isPlaying = true;


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

            // 생성 시에만 큐에서 호출
            gameCore._OnEnable();

            gameCore._Start();


            // 라이프사이클 / 오브젝트 정보 갱신
            gameCore.RegisterUpdateScheduled();


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
                // 0. 초기화
                gameCore._InitDraw();
                
                // 1. 드로우 커맨드로 스케치
                gameCore._Sketch();

                // TODO_LATER: 레이어 별로 리소스순 정렬?
                

                // 2. 렌더러로 디바이스 드로우콜
                gameCore._Render();

                // TODO_LATER: 영상 후처리
                // gameCore._PostProcess();


                // 3. 프레젠트
                gameCore._Present();


                // 4. 커맨드 클리어 후처리
                gameCore._Clear();

            }


            // gameCore._WaitForEndOfFrame();


            // 파괴 시에만 큐에서 호출
            gameCore._OnDisable();

            gameCore._OnDestroy();


            // 라이프사이클 / 오브젝트 정보 갱신
            gameCore.QuitUpdateScheduled();

            gameCore.DestroyScheduled();
            


            if (Application::_isQuit)
                break;


            // 다음 프레임...
            
        }
    }


    Application::_isPlaying = false;
}

// TODO ...손도못댐
void DDANZIT_Finalize() {

    delete pGameTimer;
    pGameTimer = nullptr;

    SceneManager::mainScene = nullptr;

    for (Scene* scene : SceneManager::pLoadedSceneList)
    {
        SceneManager::UnloadScene(scene);
    }

    gameCore._OnDisable();
    gameCore._OnDestroy();

    gameCore.QuitUpdateScheduled();
    gameCore.DestroyScheduled();


    gameCore.FinalizeGraphicSettings();


    DestroyWnd();
}


// 내부함수들..
// 구현부
namespace 
{
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

        // TODO: 여기서 코어로 접근해야되는데.. 익명이라 안뚫리네 하핫; 방법찾기

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
            break;
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
