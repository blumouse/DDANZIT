#pragma once

#include "Color.h"

// COM 및 DirectX 인터페이스
#include <wrl/client.h>              // ComPtr
#include <d3d11.h>                   // Direct3D 11
#include <d3dcompiler.h>             // Shader 컴파일
#include <dxgi1_6.h>                 // DXGI 1.6 (Windows 10 이상 최신 스왑체인)
#include <d2d1_3.h>                  // Direct2D 1.3 (ID2D1Factory4)
#include <d2d1_3helper.h>            // D2D1::Helper 클래스들
#include <dwrite_3.h>                // DirectWrite (최신 텍스트 엔진)
#include <wincodec.h>                // WIC (이미지 로딩)

// Direct3D & DXGI
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// Direct2D & DirectWrite
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

// WIC (Windows Imaging Component)
#pragma comment(lib, "windowscodecs.lib")

#pragma comment(lib, "dxguid.lib")
#include <d2d1effects.h>

using Microsoft::WRL::ComPtr;


class D2DRenderer
{
public:
	D2DRenderer(int width, int height);

	~D2DRenderer() = default;


#pragma region Properties

private:
	int width;
	int height;


	// D3D11
	ComPtr<ID3D11Device>			m_d3dDevice;
	ComPtr<ID3D11DeviceContext>		m_d3dContext;

	// DXGI
	ComPtr<IDXGISwapChain1>			m_swapChain;

	// Direct2D 최신 인터페이스
	ComPtr<ID2D1Factory4>			m_d2dFactory;
	ComPtr<ID2D1Device4>			m_d2dDevice;
	ComPtr<ID2D1DeviceContext4>		m_d2dContext;


	// Direct2D 이전 인터페이스
	ComPtr<ID2D1HwndRenderTarget>	m_renderTarget;

	// Background
	ComPtr<ID2D1SolidColorBrush>	m_brush;

	// Offscreen RenderTarget
	ComPtr<ID2D1Bitmap1>			m_targetBitmap;


	ComPtr<ID2D1Effect>				m_colorMatrixEffect;

#pragma endregion



#pragma region Settings

public:
	bool D2DRenderInitialize(HWND hwnd);
	void D2DRenderFinalize();

	void OnResize(int width, int height);

#pragma endregion



#pragma region Render

public:
	void D2DInitDraw(Color bgColor);

	void D2DRender();		// 안쓸듯
	ComPtr<ID2D1DeviceContext4> D2DGetContext();
	ComPtr<ID2D1SolidColorBrush> D2DGetBrush();
	ComPtr<ID2D1Effect> D2DGetEffect();
	ComPtr<ID2D1Bitmap1> D2DGetBitmap();

	void D2DPresent();

#pragma endregion

};
