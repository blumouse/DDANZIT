#include "DDANZIT_Core.h"

#include "Debug.h"

#include "Color.h"

#include "SceneManager.h"
#include "Scene.h"

#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "MonoBehavior.h"

#include "Collider2D.h"
#include "Rigidbody2D.h"

#include "Camera.h"
#include "IDrawable.h"

#ifdef RENDER_MODE_WINGDI
#include "RenderHelp.h"

#endif // RENDER_MODE_WINGDI

using namespace std;

using BitmapInfo = renderHelp::BitmapInfo;


#pragma region Properties

int DDANZIT_Core::width = 0;
int DDANZIT_Core::height = 0;

#ifdef RENDER_MODE_WINGDI

vector<BitmapInfo*> DDANZIT_Core::bitmapResourceList;

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D

vector<ComPtr<ID2D1Bitmap>> DDANZIT_Core::bitmapResourceList;

vector<vector<ComPtr<IDWriteTextFormat>>> DDANZIT_Core::fontResourceList;

#endif // RENDER_MODE_DIRECT2D

#pragma endregion



#pragma region Methods

bool DDANZIT_Core::InitGraphicSettings(HWND hWnd)
{
    this->hWnd = hWnd;

#ifdef RENDER_MODE_WINGDI

    RECT rcClient = {};
    GetClientRect(hWnd, &rcClient);
    width = rcClient.right - rcClient.left;
    height = rcClient.bottom - rcClient.top;

    hFrontDC = GetDC(hWnd);
    hBackDC = CreateCompatibleDC(hFrontDC);
    hBackBitmap = CreateCompatibleBitmap(hFrontDC, width, height);

    hDefaultBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D

    d2dRenderer = new D2DRenderer(width, height);

    if (!d2dRenderer->D2DRenderInitialize(hWnd))
        return false;

    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory)
    );
    if (FAILED(hr)) return false;

    
    // 폰트용
    ComPtr<IDWriteFactory> writeFactory;
    DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(writeFactory.GetAddressOf())
    );

    ComPtr<IDWriteFactory5> writeFactory5;
    hr = writeFactory.As(&writeFactory5);
    if (FAILED(hr)) return false;

#endif // RENDER_MODE_DIRECT2D

    isInitialized = true;
    return true;
}

void DDANZIT_Core::FinalizeGraphicSettings()
{

#ifdef RENDER_MODE_WINGDI

    SelectObject(hBackDC, hDefaultBitmap);

    DeleteObject(hBackBitmap);
    DeleteDC(hBackDC);

    ReleaseDC(hWnd, hFrontDC);

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D

    if (d2dRenderer != nullptr)
    {
        d2dRenderer->D2DRenderFinalize();

        for (auto rsc : bitmapResourceList)
            rsc.Reset();

        wicFactory.Reset();

        delete d2dRenderer;
    }

#endif // RENDER_MODE_DIRECT2D

    isInitialized = false;

}


void DDANZIT_Core::_OnResize(int width, int height)
{
    this->width = width;
    this->height = height;

    if (!isInitialized)
        return;

#ifdef RENDER_MODE_WINGDI

    hBackBitmap = CreateCompatibleBitmap(hFrontDC, width, height);

    HANDLE hPrevBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);

    DeleteObject(hPrevBitmap);

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D

    d2dRenderer->OnResize(width, height);

#endif // RENDER_MODE_DIRECT2D

}

void DDANZIT_Core::_OnClose()
{
    if (isInitialized)
        FinalizeGraphicSettings();
}


int DDANZIT_Core::LoadBitmapResource(const wchar_t* filePath)
{
#ifdef RENDER_MODE_WINGDI

    if (bitmapResourceList.size() == MAX_RESOURCE_NUM)
    {
        Debug::Assert(false, "LoadBitmapResource: 이미지 수가 최대입니다.");
        return -1;
    }

    bitmapResourceList.push_back(renderHelp::CreateBitmapInfo(filePath));

    return (bitmapResourceList.size() - 1);

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D

    if (bitmapResourceList.size() == MAX_RESOURCE_NUM)
    {
        Debug::Assert(false, "LoadBitmapResource: 이미지 수가 최대입니다.");
        return -1;
    }

    ComPtr<ID2D1Bitmap> newBitmap;
    LoadBitmapFromFile(d2dRenderer->D2DGetContext().Get(), filePath, &newBitmap);

    bitmapResourceList.push_back(newBitmap);

    return (bitmapResourceList.size() - 1);

#endif // RENDER_MODE_DIRECT2D

}

#ifdef RENDER_MODE_DIRECT2D
bool DDANZIT_Core::LoadFontResource(const wchar_t* filePath)
{
    if (!writeFactory) return false;

    ComPtr<IDWriteFontCollection1> customCollection;
    std::wstring familyName;

    // 1. 폰트 파일 로드 및 컬렉션/패밀리 이름 추출
    HRESULT hr = LoadFontFromFile(writeFactory.Get(), filePath, customCollection.GetAddressOf(), familyName);
    if (FAILED(hr)) return false;

    // 2. 생성된 포맷들을 담을 내부 벡터
    std::vector<ComPtr<IDWriteTextFormat>> sizeFormats;
    sizeFormats.reserve(std::size(fontSizes));

    // 3. 배열에 정의된 각 사이즈별로 TextFormat 생성
    for (float size : fontSizes)
    {
        ComPtr<IDWriteTextFormat> textFormat;
        hr = writeFactory->CreateTextFormat(
            familyName.c_str(),
            customCollection.Get(),
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            size,
            L"ko-KR",
            textFormat.GetAddressOf()
        );

        if (SUCCEEDED(hr))
        {
            sizeFormats.push_back(textFormat);
        }
    }

    // 4. static 이중 벡터에 최종 적재 (성공한 폰트 세트만 추가)
    if (!sizeFormats.empty())
    {
        fontResourceList.push_back(sizeFormats);
        return true;
    }

    return false;
}

#endif // RENDER_MODE_DIRECT2D


HRESULT DDANZIT_Core::LoadBitmapFromFile(ID2D1DeviceContext* pContext, LPCWSTR filePath, ID2D1Bitmap** ppOutBitmap)
{
    if (!wicFactory || !pContext) return E_FAIL;
    // 뭐이리 복잡해

    ComPtr<IWICBitmapDecoder> pDecoder;
    wicFactory->CreateDecoderFromFilename(
        filePath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);


    ComPtr<IWICBitmapFrameDecode> pFrame;
    pDecoder->GetFrame(0, &pFrame);


    ComPtr<IWICFormatConverter> pConverter;
    wicFactory->CreateFormatConverter(&pConverter);

    pConverter->Initialize(
        pFrame.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0f,
        WICBitmapPaletteTypeMedianCut);


    HRESULT hr = pContext->CreateBitmapFromWicBitmap(
        pConverter.Get(), nullptr, ppOutBitmap);

    return hr;
}

#ifdef RENDER_MODE_DIRECT2D

HRESULT DDANZIT_Core::LoadFontFromFile(IDWriteFactory5* pWriteFactory5, LPCWSTR filePath, IDWriteFontCollection1** ppCollection, std::wstring& outFamilyName)
{
    ComPtr<IDWriteFontSetBuilder1> fontSetBuilder;
    HRESULT hr = pWriteFactory5->CreateFontSetBuilder(fontSetBuilder.GetAddressOf());
    if (FAILED(hr)) return hr;

    ComPtr<IDWriteFontFile> fontFile;
    hr = pWriteFactory5->CreateFontFileReference(filePath, nullptr, fontFile.GetAddressOf());
    if (FAILED(hr)) return hr;

    hr = fontSetBuilder->AddFontFile(fontFile.Get());
    if (FAILED(hr)) return hr;

    ComPtr<IDWriteFontSet> fontSet;
    hr = fontSetBuilder->CreateFontSet(fontSet.GetAddressOf());
    if (FAILED(hr)) return hr;

    hr = pWriteFactory5->CreateFontCollectionFromFontSet(fontSet.Get(), ppCollection);
    if (FAILED(hr)) return hr;

    // [핵심] 파일명만으로는 TextFormat을 만들 수 없으므로, 로드된 컬렉션에서 폰트 패밀리 이름을 직접 추출
    ComPtr<IDWriteFontFamily> fontFamily;
    hr = (*ppCollection)->GetFontFamily(0, fontFamily.GetAddressOf());
    if (SUCCEEDED(hr))
    {
        ComPtr<IDWriteLocalizedStrings> familyNames;
        hr = fontFamily->GetFamilyNames(familyNames.GetAddressOf());
        if (SUCCEEDED(hr))
        {
            UINT32 length = 0;
            familyNames->GetStringLength(0, &length);

            std::wstring name(length + 1, L'\0');
            familyNames->GetString(0, &name[0], length + 1);
            name.resize(length); // 널 문자('\0') 제거

            outFamilyName = name;
        }
    }

    return hr;
}

#endif // RENDER_MODE_DIRECT2D

#pragma endregion



#pragma region Lifecycles

queue<MonoBehavior*> DDANZIT_Core::awakeExecQueue;
queue<MonoBehavior*> DDANZIT_Core::onEnableExecQueue;
queue<MonoBehavior*> DDANZIT_Core::startExecQueue;

vector<MonoBehavior*> DDANZIT_Core::fixedUpdateExecList;

vector<MonoBehavior*> DDANZIT_Core::updateExecList;
vector<MonoBehavior*> DDANZIT_Core::lateUpdateExecList;

queue<MonoBehavior*> DDANZIT_Core::onDisableExecQueue;
queue<MonoBehavior*> DDANZIT_Core::onDestroyExecQueue;


queue<MonoBehavior*> DDANZIT_Core::registerUpdateScheduledQueue;
queue<MonoBehavior*> DDANZIT_Core::quitUpdateScheduledQueue;

queue<GameObject*> DDANZIT_Core::destroyScheduledQueue;


void DDANZIT_Core::_Awake()
{
    while (!awakeExecQueue.empty())
    {
        awakeExecQueue.front()->Awake();
        awakeExecQueue.pop();
    }
}

void DDANZIT_Core::_OnEnable()
{
    while (!onEnableExecQueue.empty())
    {
        onEnableExecQueue.front()->OnEnable();
        onEnableExecQueue.pop();
    }
}

void DDANZIT_Core::_Start()
{
    while (!startExecQueue.empty())
    {
        startExecQueue.front()->Start();
        startExecQueue.pop();
    }
}

// 얘네들 비활/파괴면 실행하지 말아야함
// strict-unity니까.. 그렇게 하자고
void DDANZIT_Core::_FixedUpdate()
{
    for (MonoBehavior* b : fixedUpdateExecList)
    {
        if (b->isActiveAndEnabled() && !b->isKilled)
            b->FixedUpdate();
    }
}


void DDANZIT_Core::_OnTrigger()
{
    // 먼저 컴포넌트 리스트 순회돌면서 충돌한거 체크.. rigid : collider로
    // 새로 충돌한건 Enter 호출 및 exec리스트에 추가
    // 기존에 충돌했던거 이번에도 충돌이면 stayExec 그대로 호출
    // 나갔으면 Exit 호출 인데..
    // rigid가 prevCollideList같은걸 들고있어야겟다 이거랑 또 비교
    // 

    static vector<MonoBehavior*> execList;
    static vector<Collider2D*> enteredColliderList;


    for (Rigidbody2D* rigid : rigidbody2DList)
    {
        if (rigid->isKilled || !rigid->isActiveAndEnabled() || rigid->bodyType == RigidBodyType2D::Static)
            continue;

        // 1. 강체 리스트 비교, 하나 집어서 기준설정


        // rigid에서 쓸 것들 캐싱
        GameObject* go = rigid->_gameObject;

        for (Component* comp : go->pComponentList)
        {
            if (MonoBehavior* targetComponent = dynamic_cast<MonoBehavior*>(comp))
                execList.push_back(targetComponent);
        }


        // 2. 강체쪽의 콜라이더 집어서 비교 (여러 개일 수 있다)
        for (Collider2D* col : rigid->attachedColliderList)
        {
            if (!col->_isTrigger)           // 이 경우는 collision에서 검사합니다 (추후)
                continue;

            // 3. 전체 콜라이더 순회
            for (Collider2D* other : collider2DList)
            {
                if (other->isKilled || !other->isActiveAndEnabled())
                    continue;

                if (other->_gameObject == go)       // 강체 자신의 콜라이더
                    continue;

                if (!col->IsNearby(other))
                    continue;

                if (!col->IsCollideWith(other))
                    continue;


                // 여기까지 왔으면 충돌

                bool isNew = true;        // 더티체크

                // 4. 와 충돌이다! 이전 프레임의 충돌여부 검사
                for (Collider2D* prev : rigid->prevCollideList)     // 미띤
                {
                    // 5.1 저번 프레임에도 충돌이었어! Stay 호출해주기
                    if (other == prev)
                    {
                        for (MonoBehavior* b : execList)
                        {
                            if (b->activeOnTriggerStay2D && b->isActiveAndEnabled())
                                b->OnTriggerStay2D(prev);
                        }

                        isNew = false;
                        prev->hasCollided = true;       // 계속 충돌상태인지의 더티체크
                    }
                }

                // 5.2 어라 새로운 놈이네 Enter 호출해주고서 기록!
                if (isNew)
                {
                    for (MonoBehavior* b : execList)
                    {
                        if (b->activeOnTriggerEnter2D && b->isActiveAndEnabled())
                            b->OnTriggerEnter2D(other);
                    }

                    // 잠시 여기다 보관해두자
                    enteredColliderList.push_back(other);
                }

            }
        }


        // 6. 나간 놈들 찾기, 저번 프레임 리스트 중 이번에 충돌 안한거 Exit 호출하고 빼버리기
        for (Collider2D* prev : rigid->prevCollideList)
        {
            if (!prev->hasCollided)
            {
                for (MonoBehavior* b : execList)
                {
                    if (b->activeOnTriggerExit2D && b->isActiveAndEnabled())
                        b->OnTriggerExit2D(prev);
                }

                rigid->prevCollideList.erase(remove(
                    rigid->prevCollideList.begin(),
                    rigid->prevCollideList.end(), prev),
                    rigid->prevCollideList.end());
            }
            else
            {
                // 충돌처리 했던녀석들은 그대로 고이 보관, 플래그 초기화
                prev->hasCollided = false;
            }
        }


        // 7. 새로 충돌한놈들 보관하기
        // 괜찮겠지? 다썼으니께
        rigid->prevCollideList.reserve(rigid->prevCollideList.size() + enteredColliderList.size());

        rigid->prevCollideList.insert(rigid->prevCollideList.end(), enteredColliderList.begin(), enteredColliderList.end());
    
        
        // 강체 하나 검사 끝;
        execList.clear();
        enteredColliderList.clear();

    }

}


void DDANZIT_Core::_Update()
{
    for (MonoBehavior* b : updateExecList)
    {
        if (b->isActiveAndEnabled() && !b->isKilled)
            b->Update();
    }
}

void DDANZIT_Core::_LateUpdate()
{
    for (MonoBehavior* b : lateUpdateExecList)
    {
        if (b->isActiveAndEnabled() && !b->isKilled)
            b->LateUpdate();
    }
}


void DDANZIT_Core::_OnDisable()
{
    while (!onDisableExecQueue.empty())
    {
        onDisableExecQueue.front()->OnDisable();
        onDisableExecQueue.pop();
    }
}

void DDANZIT_Core::_OnDestroy()
{
    while (!onDestroyExecQueue.empty())
    {
        onDestroyExecQueue.front()->OnDestroy();
        onDestroyExecQueue.pop();
    }
}


void DDANZIT_Core::RegisterUpdateScheduled()
{
    while (!registerUpdateScheduledQueue.empty())
    {
        MonoBehavior* b = registerUpdateScheduledQueue.front();

        if (b->activeFixedUpdate)
            fixedUpdateExecList.push_back(b);

        if (b->activeUpdate)
            updateExecList.push_back(b);

        if (b->activeLateUpdate)
            lateUpdateExecList.push_back(b);

        b->isInUpdateList = true;
        registerUpdateScheduledQueue.pop();
    }
}

void DDANZIT_Core::QuitUpdateScheduled()
{
    while (!quitUpdateScheduledQueue.empty())
    {
        MonoBehavior* b = quitUpdateScheduledQueue.front();

        if (b->activeFixedUpdate)
            fixedUpdateExecList.erase(remove(
                fixedUpdateExecList.begin(),
                fixedUpdateExecList.end(), b),
                fixedUpdateExecList.end());

        if (b->activeUpdate)
            updateExecList.erase(remove(
                updateExecList.begin(),
                updateExecList.end(), b),
                updateExecList.end());

        if (b->activeLateUpdate)
            lateUpdateExecList.erase(remove(
                lateUpdateExecList.begin(),
                lateUpdateExecList.end(), b),
                lateUpdateExecList.end());

        quitUpdateScheduledQueue.pop();
    }
}


void DDANZIT_Core::DestroyScheduled()
{
    while (!destroyScheduledQueue.empty())
    {
        GameObject* gameObject = destroyScheduledQueue.front();
        Scene* targetScene = gameObject->_scene;

        // 컴포넌트 리스트 탈퇴... 는 개별 소멸자에서 할까

        // 씬 / 하이라키(루트)에서 빼버리기
        targetScene->RemoveFromHierarchy(gameObject);


        // TODO_LATER: 큐가 (게임아닌)오브젝트를 받는다면... 컴포넌트인 경우의 처리(분기)


        // 삭제!
        delete gameObject;
        destroyScheduledQueue.pop();
    }
}


void DDANZIT_Core::RegisterUpdateExecLists(MonoBehavior* behavior)
{
    // 오기전에 걸렀어
    //if (behavior->isInUpdateList)
    //    return;

    registerUpdateScheduledQueue.push(behavior);
    behavior->isInUpdateList = true;
}

// 지금은 파괴 시에만 호출
void DDANZIT_Core::QuitUpdateExecLists(MonoBehavior* behavior)
{
    quitUpdateScheduledQueue.push(behavior);
}

#pragma endregion



#pragma region Physics

vector<Collider2D*> DDANZIT_Core::collider2DList;
vector<Rigidbody2D*> DDANZIT_Core::rigidbody2DList;

queue<Collider2D*> DDANZIT_Core::registerCollider2DScheduledQueue;
queue<Collider2D*> DDANZIT_Core::quitCollider2DScheduledQueue;

queue<Rigidbody2D*> DDANZIT_Core::registerRigidbody2DScheduledQueue;
queue<Rigidbody2D*> DDANZIT_Core::quitRigidbody2DScheduledQueue;



void DDANZIT_Core::RegisterCollider2DScheduled()
{
    while (!registerCollider2DScheduledQueue.empty())
    {
        Collider2D* col = registerCollider2DScheduledQueue.front();

        collider2DList.push_back(col);

        registerCollider2DScheduledQueue.pop();
    }
}

void DDANZIT_Core::QuitCollider2DScheduled()
{
    while (!quitCollider2DScheduledQueue.empty())
    {
        Collider2D* col = quitCollider2DScheduledQueue.front();

        collider2DList.erase(remove(
            collider2DList.begin(),
            collider2DList.end(), col),
            collider2DList.end());

        quitCollider2DScheduledQueue.pop();
    }
}


void DDANZIT_Core::RegisterRigidbody2DScheduled()
{
    while (!registerRigidbody2DScheduledQueue.empty())
    {
        Rigidbody2D* rigid = registerRigidbody2DScheduledQueue.front();

        rigidbody2DList.push_back(rigid);

        registerRigidbody2DScheduledQueue.pop();
    }
}

void DDANZIT_Core::QuitRigidbody2DScheduled()
{
    while (!quitRigidbody2DScheduledQueue.empty())
    {
        Rigidbody2D* rigid = quitRigidbody2DScheduledQueue.front();

        rigidbody2DList.erase(remove(
            rigidbody2DList.begin(),
            rigidbody2DList.end(), rigid),
            rigidbody2DList.end());

        quitRigidbody2DScheduledQueue.pop();
    }
}


void DDANZIT_Core::RegisterCollider2DList(Collider2D* collider)
{
    registerCollider2DScheduledQueue.push(collider);
}

void DDANZIT_Core::QuitCollider2DList(Collider2D* collider)
{
    quitCollider2DScheduledQueue.push(collider);
}


void DDANZIT_Core::RegisterRigidbody2DList(Rigidbody2D* rigidbody)
{
    registerRigidbody2DScheduledQueue.push(rigidbody);
}

void DDANZIT_Core::QuitRigidbody2DList(Rigidbody2D* rigidbody)
{
    quitRigidbody2DScheduledQueue.push(rigidbody);
}


#pragma endregion



#pragma region Render

vector<IDrawable*> DDANZIT_Core::drawableList;

vector<DrawCommand> DDANZIT_Core::drawCommandLists[MAX_LAYER_NUM];
vector<UIDrawCommand> DDANZIT_Core::UIDrawCommandLists[MAX_LAYER_NUM];
#ifdef USE_DEBUG
vector<DebugDrawCommand> DDANZIT_Core::debugDrawCommandLists[MAX_LAYER_NUM];

#endif // USE_DEBUG


void DDANZIT_Core::RegisterDrawable(IDrawable* drawable) 
{
    drawableList.push_back(drawable);
}

void DDANZIT_Core::QuitDrawable(IDrawable* drawable) 
{
    drawableList.erase(remove(
        drawableList.begin(),
        drawableList.end(), drawable),
        drawableList.end());
}


void DDANZIT_Core::_InitDraw()
{

#ifdef RENDER_MODE_WINGDI

    if (Camera::currentCamera == nullptr)
        ::PatBlt(hBackDC, 0, 0, width, height, BLACKNESS);
    else
    {
        const Color& c = Camera::currentCamera->backgroundColor();

        HBRUSH hGrayBrush = CreateSolidBrush(RGB((int)(c.r * 255.0f), (int)(c.g * 255.0f), (int)(c.b * 255.0f)));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hBackDC, hGrayBrush);

        ::PatBlt(hBackDC, 0, 0, width, height, PATCOPY);

        SelectObject(hBackDC, hOldBrush);
        DeleteObject(hGrayBrush);
    }

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D

    if (Camera::currentCamera == nullptr)
        d2dRenderer->D2DInitDraw(Color{ 0.0f, 0.0f, 0.0f, 0.0f });
    else
        d2dRenderer->D2DInitDraw(Camera::currentCamera->backgroundColor());

#endif // RENDER_MODE_DIRECT2D
}

void DDANZIT_Core::_Sketch()
{
    for (IDrawable* drawable : drawableList)
        drawable->Draw();
}

void DDANZIT_Core::_Render()
{
    if (Camera::currentCamera == nullptr)
        return;

#ifdef RENDER_MODE_WINGDI

    Camera::currentCamera->Render(hBackDC);

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D

    Camera::currentCamera->Render(d2dRenderer->D2DGetContext().Get(), d2dRenderer->D2DGetBrush().Get(), d2dRenderer->D2DGetEffect().Get(), writeFactory.Get());

#endif // RENDER_MODE_DIRECT2D

}

void DDANZIT_Core::_Present()
{

#ifdef RENDER_MODE_WINGDI

    BitBlt(hFrontDC, 0, 0, width, height, hBackDC, 0, 0, SRCCOPY);

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D

    d2dRenderer->D2DPresent();

#endif // RENDER_MODE_DIRECT2D

}

void DDANZIT_Core::_Clear()
{
    for (int i = MAX_LAYER_NUM - 1; i >= 0; i--)
    {
        drawCommandLists[i].clear();
        debugDrawCommandLists[i].clear();
    }
}

#pragma endregion