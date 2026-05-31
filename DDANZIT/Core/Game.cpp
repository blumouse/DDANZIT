//#include "Game.h"
//#include "GameTimer.h"
//#include "MyGameObject.h"
//#include "IDrawable.h"
//#include "Transform.h"
//
//#include <iostream>
//#include <assert.h>
//#include "RenderHelp.h"
//
//#include "GameManagerObject.h"
//#include "ClickableManagerObject.h"
//
//#include "ClickableManager.h"
//
//using namespace learning;
//
//// 어디부터 만드는게 나을까..?
//// 프레임워크부터 만드는게 낫겠지? 대강 호출순서로 위에부터
//// 0.01% 유니티
//
//// 크게 프레임워크 구현 / 클릭ux 구현 / 게임로직 구현 으로 나누자
//// 아 비트맵도 연결해야됨;
//
//// TODO: 
//// 게임오버 시 나.. 아 어쨌든 ui / 버튼같은것들 추가 (여유되면) (팝업 ui같은게.. 되나? 지금구조에서.. 걍 생성을 새로 하면되긴 할듯)
//// 클릭 ux.. 이건 기획부터 이슈네 사실; 기본 할만큼은 해놨어 사실
//
//
//// pch 하나 묶어서 놔둘까?
//// 여유되면 friends로 뺄거도 생각해보기 (주로 Game에서 가져갈거)
//// 이슈; RegisterObject 호출이 유저코드 단으로 나오게 하고싶지 않은데요........
//// 팩토리로 해버려?? 제네릭으로 타입받아서 뉴 되나? 뉴 자체를 숨기기
//
//
//constexpr int MAX_GAME_OBJECT_COUNT = 1000;
//
//
//Game* Game::instance = nullptr;
//
//Game* Game::GetInstance() { return Game::instance; }
//
//bool Game::Initialize()
//{
//    // 싱글톤
//    if (instance != nullptr && instance != this)
//    {
//        return false;
//    }
//    else /*if (instance == nullptr)*/
//    {
//        instance = this;
//    }
//
//
//    // 초초기화 0
//    pGameTimer = new GameTimer();
//    pGameTimer->Reset();
//
//    const wchar_t* className = L"MiniGame";
//    const wchar_t* windowName = L"지뢰 찾기";
//
//    if (false == __super::Create(className, windowName, 420, 460))
//    {
//        return false;
//    }
//
//
//    // 초초기화 1
//    RECT rcClient = {};
//    GetClientRect(m_hWnd, &rcClient);
//    m_width = rcClient.right - rcClient.left;
//    m_height = rcClient.bottom - rcClient.top;
//
//    m_hFrontDC = GetDC(m_hWnd);
//    m_hBackDC = CreateCompatibleDC(m_hFrontDC);
//    m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);
//
//    m_hDefaultBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);
//
//
//    // 초초기화 2
//    if (!LoadResources())
//    {
//        return false;
//    }
//
//
//    // 게임 초기화
//    ppGameObjects = new GameObjectBase* [MAX_GAME_OBJECT_COUNT];
//    ppTransforms = new Transform* [MAX_GAME_OBJECT_COUNT];
//
//    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
//    {
//        ppGameObjects[i] = nullptr;
//        ppTransforms[i] = nullptr;
//    }
//
//    gameObjectsIndex = 0;
//
//    for (int i = 0; i < MAX_LAYER_NUM; i++)
//    {
//        ppDrawableLayers[i] = new IDrawable* [MAX_GAME_OBJECT_COUNT];
//
//        for (int j = 0; j < MAX_GAME_OBJECT_COUNT; j++)
//            ppDrawableLayers[i][j] = nullptr;
//    }
//
//
//    // 생성!!
//    // 유니티로 치면.. 하이라키에 올라가있는 오브젝트들
//    // ... 하이라키를 만들어버릴수도 있겠는데
//
//    ClickableManagerObject* cmo = new ClickableManagerObject();
//    //RegisterObject(cmo);
//
//    GameManagerObject* gmo = new GameManagerObject();
//    //RegisterObject(gmo);
//
//    // 패널은 게임매니저에서 생성(인스턴트)..!
//    // Awake 내에서 일단 해야겟군
//
//
//    // 다하고서 Start
//    Start();
//    isStarted = true;       // 이후에 생성되는 놈들은 따로 호출해줌
//
//
//    return true;
//}
//
//
//
//void Game::Run()
//{
//    MSG msg = { 0 };
//    while (msg.message != WM_QUIT)
//    {
//        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
//        {
//            if (msg.message == WM_MOUSEMOVE)
//            {
//                Game::OnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));
//            }
//            else if (msg.message == WM_LBUTTONDOWN)
//            {
//                Game::OnLButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
//            }
//            else if (msg.message == WM_RBUTTONDOWN)
//            {
//                Game::OnRButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
//            }
//            else if (msg.message == WM_LBUTTONUP)
//            {
//                Game::OnLButtonUp(LOWORD(msg.lParam), HIWORD(msg.lParam));
//            }
//            else if (msg.message == WM_RBUTTONUP)
//            {
//                Game::OnRButtonUp(LOWORD(msg.lParam), HIWORD(msg.lParam));
//            }
//            else
//            {
//                TranslateMessage(&msg);
//                DispatchMessage(&msg);
//            }
//        }
//        else
//        {
//            _Update();
//            _Render();
//        }
//    }
//}
//
//
//void Game::Finalize()
//{
//    delete pGameTimer;
//    pGameTimer = nullptr;
//
//    if (ppTransforms)
//    {
//        delete ppTransforms;
//        ppTransforms = nullptr;
//    }
//
//    for (int i = 0; i < MAX_LAYER_NUM; i++)
//    {
//        if (ppDrawableLayers[i])
//        {
//            delete ppDrawableLayers[i];
//            ppDrawableLayers[i] = nullptr;
//        }
//    }
//
//    if (ppGameObjects)
//    {
//        for (int i = 0; i < gameObjectsIndex; ++i)
//        {
//            if (ppGameObjects[i])
//            {
//                ppGameObjects[i]->OnDestroy();
//
//                delete ppGameObjects[i];
//                ppGameObjects[i] = nullptr;
//            }
//        }
//        delete ppGameObjects;
//        ppGameObjects = nullptr;
//    }
//
//    //delete ppBitmapResources;
//
//    __super::Destroy();
//}
//
//
//void Game::_Update()
//{
//    pGameTimer->Tick();
//
//    Update();
//
//    fDeltaTime = pGameTimer->DeltaTimeMS();
//    fFrameCount += fDeltaTime;
//
//    while (fFrameCount >= 200.0f)
//    {
//        FixedUpdate();
//        fFrameCount -= 200.0f;
//    }
//}
//
//
//void Game::_Render()
//{
//    HBRUSH hGrayBrush = CreateSolidBrush(RGB(216, 216, 216));
//    HBRUSH hOldBrush = (HBRUSH)SelectObject(m_hBackDC, hGrayBrush);
//
//    ::PatBlt(m_hBackDC, 0, 0, m_width, m_height, PATCOPY); 
//
//    SelectObject(m_hBackDC, hOldBrush);
//    DeleteObject(hGrayBrush);
//    
//
//    // 0번 레이어가 가장 위
//    for (int i = MAX_LAYER_NUM - 1; i >= 0; i--)
//    {
//        for (int j = 0; j < gameObjectsIndex; ++j)
//        {
//            if (ppDrawableLayers[i][j])
//                ppDrawableLayers[i][j]->Draw(m_hBackDC);
//        }
//    }
//
//
//    //메모리 DC에 그려진 결과를 실제 DC(m_hFrontDC)로 복사
//    BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
//}
//
//
////void Game::Awake() 
////{
////    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
////    {
////        if (ppGameObjects[i])
////        {
////            ppGameObjects[i]->Awake();
////        }
////        else
////            break;
////    }
////}
//
//
//void Game::Start()
//{
//    for (int i = 0; i < gameObjectsIndex; ++i)
//    {
//        if (ppGameObjects[i])
//        {
//            ppGameObjects[i]->Start();
//        }
//    }
//}
//
//
//void Game::Update()
//{
//    for (int i = 0; i < gameObjectsIndex; ++i)
//    {
//        if (ppGameObjects[i])
//        {
//            ppGameObjects[i]->Update(fDeltaTime);
//        }
//    }
//}
//
//
//void Game::FixedUpdate()
//{
//    for (int i = 0; i < gameObjectsIndex; ++i)
//    {
//        if (ppGameObjects[i])
//        {
//            ppGameObjects[i]->FixedUpdate();
//        }
//    }
//}
//
//
//void Game::OnMouseMove(int x, int y)
//{
//    ClickableManager* cm = nullptr;
//
//    if (cm = ClickableManager::GetInstance())
//        cm->HandleMouseMove(x, y);
//}
//
//void Game::OnLButtonDown(int x, int y)
//{
//    ClickableManager* cm = nullptr;
//
//    if (cm = ClickableManager::GetInstance())
//        cm->HandleLDown(x, y);
//}
//
//void Game::OnRButtonDown(int x, int y)
//{
//    ClickableManager* cm = nullptr;
//
//    if (cm = ClickableManager::GetInstance())
//        cm->HandleRDown(x, y);
//}
//
//void Game::OnLButtonUp(int x, int y)
//{
//    ClickableManager* cm = nullptr;
//
//    if (cm = ClickableManager::GetInstance())
//        cm->HandleLUp(x, y);
//}
//
//void Game::OnRButtonUp(int x, int y)
//{
//    ClickableManager* cm = nullptr;
//
//    if (cm = ClickableManager::GetInstance())
//        cm->HandleRUp(x, y);
//}
//
//
//void Game::OnResize(int width, int height)
//{
//    std::cout << __FUNCTION__ << std::endl;
//
//    learning::SetScreenSize(width, height);
//
//    __super::OnResize(width, height);
//
//    m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);
//
//    HANDLE hPrevBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);
//
//    DeleteObject(hPrevBitmap);
//}
//
//
//void Game::OnClose()
//{
//    std::cout << __FUNCTION__ << std::endl;
//
//    SelectObject(m_hBackDC, m_hDefaultBitmap);
//
//    DeleteObject(m_hBackBitmap);
//    DeleteDC(m_hBackDC);
//
//    ReleaseDC(m_hWnd, m_hFrontDC);
//}
//
//
//bool Game::LoadResources() 
//{
//    // TODO
//    // 원래는 인자로 뭐 문자열 받아서 로드하고 그래야겠지
//    // 로드해주고 핸들(주소) 돌려주고
//    //ppBitmapResources = new BitmapInfo* [MAX_BMI_NUM];
//
//    //while (bmiIndex != MAX_BMI_NUM)
//
//    //ppBitmapResources[bmiIndex++] = renderHelp::CreateBitmapInfo(L"./Resource/background.png");
//    ppBitmapResources[bmiIndex++] = renderHelp::CreateBitmapInfo(L"./Resource/mine.png");
//    ppBitmapResources[bmiIndex++] = renderHelp::CreateBitmapInfo(L"./Resource/number.png");
//    ppBitmapResources[bmiIndex++] = renderHelp::CreateBitmapInfo(L"./Resource/face.png");
//
//    return true;
//}
//
//Game::BitmapInfo* Game::GetBitmapResource(int index) {
//    if (index >= bmiIndex || index < 0)
//        return nullptr;
//
//    return ppBitmapResources[index];
//}
//
//void Game::RegisterObject(GameObjectBase* gameObject)
//{
//    // 효율적인 검색을 위한..
//
//    for (int i = 0; i < gameObjectsIndex; i++)
//    {
//        if (ppGameObjects[i] == nullptr)
//        {
//            ppGameObjects[i] = gameObject;
//            gameObject->Awake();
//
//            // 아몰라
//            if (isStarted)
//                gameObject->Start();
//
//            return;
//        }
//    }
//
//
//    if (gameObjectsIndex == MAX_GAME_OBJECT_COUNT)
//        // 안돼
//        return;
//
//    ppGameObjects[gameObjectsIndex++] = gameObject;
//    gameObject->Awake();
//
//    if (isStarted)
//        gameObject->Start();
//}
//
//void Game::RegisterTransform(Transform* transform)
//{
//    for (int i = 0; i < gameObjectsIndex; i++)
//    {
//        if (ppTransforms[i] == nullptr)
//        {
//            ppTransforms[i] = transform;
//            break;
//        }
//    }
//}
//
//
//void Game::RegisterDrawable(IDrawable* drawable)
//{
//    for (int i = 0; i < gameObjectsIndex; i++)
//    {
//        if (ppDrawableLayers[0][i] == nullptr)
//        {
//            ppDrawableLayers[0][i] = drawable;
//            break;
//        }
//    }
//}
//
//void Game::RegisterDrawable(IDrawable* drawable, int layer) 
//{
//    if (layer < 0 || layer > MAX_LAYER_NUM)
//        return;
//
//    for (int i = 0; i < gameObjectsIndex; i++)
//    {
//        if (ppDrawableLayers[layer][i] == nullptr)
//        {
//            ppDrawableLayers[layer][i] = drawable;
//            break;
//        }
//    }
//}
//
//void Game::QuitDrawable(IDrawable* drawable)
//{
//    for (int i = 0; i < MAX_LAYER_NUM; i++)
//    {
//        for (int j = 0; j < gameObjectsIndex; j++)
//        {
//            if (ppDrawableLayers[i][j] && ppDrawableLayers[i][j] == drawable)
//            {
//                ppDrawableLayers[i] = nullptr;
//                break;
//            }
//        }
//    }
//}
//
//void Game::QuitDrawable(IDrawable* drawable, int layer) 
//{
//    for (int i = 0; i < gameObjectsIndex; i++)
//    {
//        if (ppDrawableLayers[layer][i] && ppDrawableLayers[layer][i] == drawable)
//        {
//            ppDrawableLayers[layer] = nullptr;
//            break;
//        }
//    }
//}
//
//void Game::DestroyObject(GameObjectBase* gameObject) 
//{
//    IDrawable* drawable = dynamic_cast<IDrawable*>(gameObject);
//    if (drawable)
//        QuitDrawable(drawable, drawable->GetLayer());
//
//    for (int i = 0; i < gameObjectsIndex; i++)
//    {
//        if (ppTransforms[i] && ppTransforms[i] == dynamic_cast<Transform*>(gameObject))
//        {
//            ppTransforms[i] = nullptr;
//            break;
//        }
//    }
//
//    for (int i = 0; i < gameObjectsIndex; i++)
//    {
//        if (ppGameObjects[i] && ppGameObjects[i] == gameObject)
//        {
//            ppGameObjects[i] = nullptr;
//
//            gameObject->OnDestroy();
//            delete gameObject;
//
//            return;
//        }
//    }
//
//    // 여기 왔으면.. 너 뭐임
//    assert(gameObject == nullptr);
//}
//
//
//GameObjectBase* Game::GetObjectWithPos(int mouseX, int mouseY)
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
//bool Game::TryGetObjectWithPos(int mouseX, int mouseY, GameObjectBase*& pGameObject)
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