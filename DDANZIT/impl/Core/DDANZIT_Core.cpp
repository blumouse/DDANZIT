#include "DDANZIT_Core.h"

#include "Color.h"

#include "SceneManager.h"
#include "Scene.h"

#include "Camera.h"
#include "IDrawable.h"

#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "MonoBehavior.h"

#include "RenderHelp.h"

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

#endif // RENDER_MODE_DIRECT2D

#pragma endregion



#pragma region Methods

void DDANZIT_Core::InitGraphicSettings(HWND hWnd)
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

    CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory)
    );

#endif // RENDER_MODE_DIRECT2D

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

    d2dRenderer->D2DRenderFinalize();

    wicFactory->Release();

    if (d2dRenderer != nullptr)
        delete d2dRenderer;

#endif // RENDER_MODE_DIRECT2D

}


void DDANZIT_Core::_OnResize(int width, int height)
{
    this->width = width;
    this->height = height;

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
#ifdef RENDER_MODE_WINGDI

    SelectObject(hBackDC, hDefaultBitmap);

    DeleteObject(hBackBitmap);
    DeleteDC(hBackDC);

    ReleaseDC(hWnd, hFrontDC);

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D

    d2dRenderer->D2DRenderFinalize();

    if (d2dRenderer != nullptr)
        delete d2dRenderer;

#endif // RENDER_MODE_DIRECT2D

}


int DDANZIT_Core::LoadBitmapResource(const wchar_t* filePath)
{
#ifdef RENDER_MODE_WINGDI

    if (bitmapResourceList.size() == MAX_RESOURCE_NUM)
    {
        // DEBUG: 꽉찻어
        return -1;
    }

    bitmapResourceList.push_back(renderHelp::CreateBitmapInfo(filePath));

    return (bitmapResourceList.size() - 1);

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D

    if (bitmapResourceList.size() == MAX_RESOURCE_NUM)
    {
        // DEBUG: 꽉찻어
        return -1;
    }

    ComPtr<ID2D1Bitmap> newBitmap;
    LoadBitmapFromFile(d2dRenderer->D2DGetContext().Get(), filePath, &newBitmap);

    bitmapResourceList.push_back(newBitmap);

    return (bitmapResourceList.size() - 1);

#endif // RENDER_MODE_DIRECT2D

}


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
        if (b->isActiveAndEnabled() && !b->gameObject()->isKilled)
            b->FixedUpdate();
    }
}

void DDANZIT_Core::_Update()
{
    for (MonoBehavior* b : updateExecList)
    {
        if (b->isActiveAndEnabled() && !b->gameObject()->isKilled)
            b->Update();
    }
}

void DDANZIT_Core::_LateUpdate()
{
    for (MonoBehavior* b : lateUpdateExecList)
    {
        if (b->isActiveAndEnabled() && !b->gameObject()->isKilled)
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


        // 씬 / 하이라키(루트)에서 빼버리기
        targetScene->RemoveFromHierarchy(gameObject);


        // TODO_LATER: 큐가 (게임아닌)오브젝트를 받는다면... 컴포넌트인 경우의 처리(분기)


        // 삭제!
        delete destroyScheduledQueue.front();
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



#pragma region Render

vector<IDrawable*> DDANZIT_Core::drawableList;

vector<DrawCommand> DDANZIT_Core::drawCommandLists[MAX_LAYER_NUM];
vector<DebugDrawCommand> DDANZIT_Core::debugDrawCommandLists[MAX_LAYER_NUM];


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
    // TODO: 메인 씬 배경색으로 칠

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

    Camera::currentCamera->Render(d2dRenderer->D2DGetContext());

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
    }
}

#pragma endregion