#pragma once

#ifdef RENDER_MODE_WINGDI
typedef struct HDC__* HDC;

#endif

#ifdef RENDER_MODE_DIRECT2D
#include "D2DRenderer.h"

#endif


class IRenderer
{
protected:
	IRenderer() = default;

public:
	IRenderer(const IRenderer& other) = default;
	virtual ~IRenderer() = default;


	// 근데 이러면 인터페이스 쓰는 의미가..?
#ifdef RENDER_MODE_WINGDI
	virtual void Render(HDC hdc) = 0;

#endif

#ifdef RENDER_MODE_DIRECT2D
	virtual void Render(ID2D1DeviceContext4* d2dcontext, ID2D1SolidColorBrush* d2dbrush, ID2D1Bitmap1* d2dtargetBitmap) = 0;

#endif

};