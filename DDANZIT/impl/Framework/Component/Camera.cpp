#include "Camera.h"

#include "DefineOption.h"
#include "DDANZIT_Core.h"

#include "SceneManager.h"
#include "Scene.h"

#include "GameObject.h"
#include "Transform.h"

#ifdef RENDER_MODE_WINGDI
#include "RenderHelp.h"

using BitmapInfo = renderHelp::BitmapInfo;

#endif // RENDER_MODE_WINGDI

using namespace std;


#pragma region Constructor

Camera::Camera(GameObject* pGameObject) :
	Component(pGameObject)
{
	RegisterCameraList(this);
}


Camera::~Camera()
{
	QuitCameraList(this);
}

#pragma endregion



#pragma region Clone

Component* Camera::Clone() const
{
	return new Camera(*this);
}

#pragma endregion



#pragma region Properties

Color& Camera::backgroundColor()
{
	if (_scene == nullptr)
		return SceneManager::GetActiveScene()->backgroundColor();
	else
		return _scene->backgroundColor();
}

const Color& Camera::backgroundColor() const
{
	if (_scene == nullptr)
		return SceneManager::GetActiveScene()->backgroundColor();
	else
		return _scene->backgroundColor();
}


void Camera::SetDepth(int depth)
{
#ifdef PROPS_MODE_2D
	transform()->SetDepth(depth);

#endif // PROPS_MODE_2D
}

int Camera::depth()
{
#ifdef PROPS_MODE_2D
	return transform()->depth();

#endif // PROPS_MODE_2D
}


#pragma endregion




#ifdef RENDER_MODE_WINGDI

#pragma region IRenderer

void Camera::Render(HDC hdc)
{
	// 카메라 단 오브젝트의 뎁스부터 컬링
#ifdef PROPS_MODE_2D
	
	Vector2 camPos = transform()->position();
	float camPosX = camPos.x;
	float camPosY = camPos.y;
	static struct SquareSprite {
		HDC hDC = NULL;
		HBITMAP hBmp = NULL;
		HBITMAP hOldBmp = NULL;

		SquareSprite() {
			HDC hScreenDC = GetDC(NULL);
			hDC = CreateCompatibleDC(hScreenDC);
			hBmp = CreateCompatibleBitmap(hScreenDC, 1, 1);
			hOldBmp = (HBITMAP)SelectObject(hDC, hBmp);
			ReleaseDC(NULL, hScreenDC);
		}

		~SquareSprite() {
			if (hDC) {
				SelectObject(hDC, hOldBmp);
				DeleteObject(hBmp);
				DeleteDC(hDC);
			}
		}
	} squareSprite;


	for (int i = MAX_LAYER_NUM - 1; i >= depth(); i--)
	{
		for (const DrawCommand& cmd : DDANZIT_Core::drawCommandLists[i])
		{
			if ((int)cmd.spriteIndex < 0)
			{
				HDC defalutSpriteHDC = NULL;

				float relativeCamX = cmd.posX - camPosX;
				float relativeCamY = cmd.posY - camPosY;

				int x;
				int y;

				float relativeScaleX;
				float relativeScaleY;

				int srcX;	int srcWidth;
				int srcY;	int srcHeight;

				int xBGR;

				switch (cmd.spriteIndex)
				{
				default:
				case SpriteIndex::None:
					continue;

				case SpriteIndex::Sqaure:
					// 직접 그려서 만들고 캐싱

					SetPixel(squareSprite.hDC, 0, 0, RGB(((cmd.colorRGBA >> 24) & 0x000000ff), ((cmd.colorRGBA >> 16) & 0x000000ff), ((cmd.colorRGBA >> 8) & 0x000000ff)));

					defalutSpriteHDC = squareSprite.hDC;

					relativeScaleX = cmd.scaleX;
					relativeScaleY = cmd.scaleY;

					x = (int)((relativeCamX - relativeScaleX / 2.0f) * worldToScreenRatio) + DDANZIT_Core::width / 2;
					y = (int)((relativeCamY - relativeScaleY / 2.0f) * worldToScreenRatio) + DDANZIT_Core::height / 2;


					srcX = 0;
					srcY = 0;

					srcWidth = 1;
					srcHeight = 1;

					break;

				case SpriteIndex::Circle:

					break;

				case SpriteIndex::Capsule:

					break;
				}



				BLENDFUNCTION blend = { 0 };
				blend.BlendOp = AC_SRC_OVER;
				blend.SourceConstantAlpha = (cmd.colorRGBA & 0x000000ff);
				blend.AlphaFormat = 0;

				AlphaBlend(hdc, x, y, relativeScaleX * worldToScreenRatio, relativeScaleY * worldToScreenRatio,
					defalutSpriteHDC, srcX, srcY, srcWidth, srcHeight, blend);
			}
			else
			{
				BitmapInfo* bmi = DDANZIT_Core::bitmapResourceList[(int)cmd.spriteIndex];

				if (bmi == nullptr) continue;
				if (bmi->GetBitmapHandle() == nullptr) continue;

				HDC hBitmapDC = CreateCompatibleDC(hdc);

				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hBitmapDC, bmi->GetBitmapHandle());

				// 카메라 + 화면 중심 좌표로 계산
				// 카메라 벡터는 빼고 화면 중심좌표 계산해서 더해 (y축 뒤집어야되나?)
				// 아 하나더 원본 비트맵 기준으로 배율을 주는거로 계산..
				
				float relativeCamX = cmd.posX - camPosX;
				float relativeCamY = cmd.posY - camPosY;

				BITMAP bmp;
				float relativeScaleX;
				float relativeScaleY;

				if (cmd.useAtlas)
				{
					relativeScaleX = cmd.scaleX * (float)cmd.sliceWidth / worldToScreenRatio;
					relativeScaleY = cmd.scaleY * (float)cmd.sliceHeight / worldToScreenRatio;
				}
				else
				{
					GetObject(bmi->GetBitmapHandle(), sizeof(BITMAP), &bmp);

					relativeScaleX = cmd.scaleX * (float)bmp.bmWidth / worldToScreenRatio;
					relativeScaleY = cmd.scaleY * (float)bmp.bmHeight / worldToScreenRatio;
				}

				int x = (int)((relativeCamX - relativeScaleX / 2.0f) * worldToScreenRatio) + DDANZIT_Core::width / 2;
				int y = (int)((relativeCamY - relativeScaleY / 2.0f) * worldToScreenRatio) + DDANZIT_Core::height / 2;


				int srcX;	int srcWidth;
				int srcY;	int srcHeight;

				if (cmd.useAtlas)
				{
					srcX = cmd.sliceWidth * cmd.sliceIndexX;
					srcY = cmd.sliceHeight * cmd.sliceIndexY;

					srcWidth = cmd.sliceWidth;
					srcHeight = cmd.sliceHeight;
				}
				else
				{
					srcX = 0;
					srcY = 0;

					srcWidth = bmp.bmWidth;
					srcHeight = bmp.bmHeight;
				}

				// GDI로 다른색상 곱하기는 무리
				BLENDFUNCTION blend = { 0 };
				blend.BlendOp = AC_SRC_OVER;
				blend.SourceConstantAlpha = (cmd.colorRGBA & 0x000000ff);
				blend.AlphaFormat = AC_SRC_ALPHA;

				// 회전도 무리 dir 안써!

				// TODO: 플립 반영하기 (파라미터만 뒤집음 된다)

				AlphaBlend(hdc, x, y, relativeScaleX * worldToScreenRatio, relativeScaleY * worldToScreenRatio,
					hBitmapDC, srcX, srcY, srcWidth, srcHeight, blend);


				SelectObject(hBitmapDC, hOldBitmap);
				DeleteDC(hBitmapDC);
			}

		}
	}


	// TODO: 콜라이더 선 그리기
	for (int i = MAX_LAYER_NUM - 1; i >= depth(); i--)
	{
		for (const DebugDrawCommand& cmd : DDANZIT_Core::debugDrawCommandLists[i])
		{

		}
	}

#endif // PROPS_MODE_2D

}

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D

void Camera::Render(ComPtr<ID2D1DeviceContext4> d2dcontext, ComPtr<ID2D1SolidColorBrush> d2dbrush, ComPtr<ID2D1Bitmap1> d2dtargetBitmap)
{
	// 카메라 단 오브젝트의 뎁스부터 컬링
#ifdef PROPS_MODE_2D

	Vector2 camPos = transform()->position();
	float camPosX = camPos.x;
	float camPosY = camPos.y;


	for (int i = MAX_LAYER_NUM - 1; i >= depth(); i--)
	{
		for (const DrawCommand& cmd : DDANZIT_Core::drawCommandLists[i])
		{
			if ((int)cmd.spriteIndex < 0)
			{
				if (cmd.spriteIndex == SpriteIndex::None)
					continue;


				float relativeCamX = cmd.posX - camPosX;
				float relativeCamY = cmd.posY - camPosY;

				float x;
				float y;

				float relativeScaleX;
				float relativeScaleY;

				UINT32 rgb;
				float a;
				rgb = (cmd.colorRGBA >> 8) & 0x00ffffff;
				a = (float)(cmd.colorRGBA & 0x000000ff) / 255.0f;

				d2dbrush->SetColor(D2D1::ColorF(rgb, a));

				if (cmd.angle != 0.0f)
				{
					D2D1_MATRIX_3X2_F rotMatrix = D2D1::Matrix3x2F::Rotation(cmd.angle, D2D1::Point2F(relativeCamX, relativeCamY));
					d2dcontext->SetTransform(rotMatrix);
				}
				// 플립은 하나마나니까 생략;

				switch (cmd.spriteIndex)
				{
				default:	// ?
					continue;

				case SpriteIndex::Sqaure:

					relativeScaleX = cmd.scaleX;
					relativeScaleY = cmd.scaleY;

					x = ((relativeCamX - relativeScaleX / 2.0f) * worldToScreenRatio) + (float)DDANZIT_Core::width / 2.0f;
					y = ((relativeCamY - relativeScaleY / 2.0f) * worldToScreenRatio) + (float)DDANZIT_Core::height / 2.0f;


					d2dcontext->FillRectangle(D2D1::RectF(x, y, x + relativeScaleX * worldToScreenRatio, y + relativeScaleY * worldToScreenRatio), d2dbrush.Get());

					break;

				case SpriteIndex::Circle:

					relativeScaleX = cmd.scaleX;
					relativeScaleY = cmd.scaleY;

					x = relativeCamX * worldToScreenRatio + (float)DDANZIT_Core::width / 2.0f;
					y = relativeCamY * worldToScreenRatio + (float)DDANZIT_Core::height / 2.0f;


					d2dcontext->FillEllipse(
						D2D1::Ellipse(
							D2D1::Point2F(x, y),
							(relativeScaleX / 2.0f) * worldToScreenRatio,
							(relativeScaleY / 2.0f) * worldToScreenRatio),
						d2dbrush.Get());

					break;

				case SpriteIndex::Capsule:

					// TODO

					break;
				}


				if (cmd.angle != 0.0f)
				{
					d2dcontext->SetTransform(D2D1::Matrix3x2F::Identity());
				}

			}
			else
			{
				ID2D1Bitmap* pBitmap = DDANZIT_Core::bitmapResourceList[(int)cmd.spriteIndex].Get();

				if (pBitmap == nullptr)
				{
					// DEBUG
					continue;
				}



				float relativeCamX = cmd.posX - camPosX;
				float relativeCamY = cmd.posY - camPosY;

				float relativeScaleX;
				float relativeScaleY;


				if (cmd.useAtlas)
				{
					relativeScaleX = cmd.scaleX * (float)cmd.sliceWidth / worldToScreenRatio;
					relativeScaleY = cmd.scaleY * (float)cmd.sliceHeight / worldToScreenRatio;
				}
				else
				{
					relativeScaleX = cmd.scaleX * pBitmap->GetPixelSize().width / worldToScreenRatio;
					relativeScaleY = cmd.scaleY * pBitmap->GetPixelSize().height / worldToScreenRatio;
				}

				int x = (int)((relativeCamX - relativeScaleX / 2.0f) * worldToScreenRatio) + DDANZIT_Core::width / 2;
				int y = (int)((relativeCamY - relativeScaleY / 2.0f) * worldToScreenRatio) + DDANZIT_Core::height / 2;


				int srcX;	int srcWidth;
				int srcY;	int srcHeight;

				if (cmd.useAtlas)
				{
					srcX = cmd.sliceWidth * cmd.sliceIndexX;
					srcY = cmd.sliceHeight * cmd.sliceIndexY;

					srcWidth = cmd.sliceWidth;
					srcHeight = cmd.sliceHeight;
				}
				else
				{
					srcX = 0;
					srcY = 0;

					srcWidth = pBitmap->GetPixelSize().width;
					srcHeight = pBitmap->GetPixelSize().height;
				}

				// 회전 반전!
				D2D1_POINT_2F centerPos = D2D1::Point2F(relativeCamX, relativeCamY);

				D2D1_MATRIX_3X2_F flipMatrix = D2D1::Matrix3x2F::Identity();
				D2D1_MATRIX_3X2_F rotMatrix = D2D1::Matrix3x2F::Identity();

				// 반전이랑 스케일링이랑 동치라고 하네요
				if (cmd.flipX)
				{
					if (cmd.flipY)
						flipMatrix = D2D1::Matrix3x2F::Scale(-1.0f, -1.0f, centerPos);
					else
						flipMatrix = D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, centerPos);
				}
				else if (cmd.flipY)
				{
					flipMatrix = D2D1::Matrix3x2F::Scale(1.0f, -1.0f, centerPos);
				}

				if (cmd.angle != 0.0f)
					rotMatrix = D2D1::Matrix3x2F::Rotation(cmd.angle, centerPos);

				d2dcontext->SetTransform(flipMatrix * rotMatrix);


				// 색상 곱하기!
				UINT32 rgb;
				float a;
				rgb = (cmd.colorRGBA >> 8) & 0x00ffffff;
				a = (float)(cmd.colorRGBA & 0x000000ff) / 255.0f;

				D2D1::ColorF blendColor = D2D1::ColorF(rgb, a);


				// 그리기~
				D2D1_RECT_F destRect = D2D1::RectF(x, y, x + relativeScaleX * worldToScreenRatio, y + relativeScaleY * worldToScreenRatio);

				// TODO: 아틀라스 반영하기
				d2dcontext->DrawBitmap(
					pBitmap,
					&destRect,
					blendColor.a,
					D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
				);


				if (cmd.angle != 0.0f || cmd.flipX || cmd.flipY)
				{
					d2dcontext->SetTransform(D2D1::Matrix3x2F::Identity());
				}

			}

		}
	}


	// TODO: 콜라이더 선 그리기
	for (int i = MAX_LAYER_NUM - 1; i >= depth(); i--)
	{
		for (const DebugDrawCommand& cmd : DDANZIT_Core::debugDrawCommandLists[i])
		{
			//d2dbrush->SetColor(D2D1::ColorF(0.0f, 1.0f, 0.0f, 0.5f));
			//d2dcontext->DrawLine(
			//	D2D1::Point2F(10.0f, 200.0f),
			//	D2D1::Point2F(300.0f, 200.0f),
			//	d2dbrush.Get(),
			//	3.0f // 선 두께
			//);
		}
	}

#endif // PROPS_MODE_2D

}

#endif // RENDER_MODE_DIRECT2D


#pragma endregion



#pragma region StaticMethod

const float Camera::worldToScreenRatio = 100.0f;

Camera* Camera::currentCamera = nullptr;

vector<Camera*> Camera::allCameraList;


Camera* Camera::current()
{
	return currentCamera;
}

void Camera::SetupCurrent(Camera* cur)
{
	if (cur == nullptr)
	{
		// DEBUG
		return;
	}

	currentCamera = cur;
}


vector<Camera*> Camera::allCameras()
{
	// 사실은 켜진것만 골라 반환한다
	vector<Camera*> enabledCameraList;

	for (Camera* cam : allCameraList)
	{
		if (!cam->isKilled && cam->isActiveAndEnabled())
			enabledCameraList.push_back(cam);
	}

	return enabledCameraList;
}

int Camera::allCamerasCount()
{
	// 이건 또 전체 수를 반환한다
	return allCameraList.size();
}


void Camera::RegisterCameraList(Camera* cam)
{
	Camera::allCameraList.push_back(cam);

	if (Camera::currentCamera == nullptr)
		Camera::SetupCurrent(cam);
}

void Camera::QuitCameraList(Camera* cam)
{
	if (currentCamera == cam)
		currentCamera = nullptr;

	allCameraList.erase(remove(
		allCameraList.begin(),
		allCameraList.end(), cam),
		allCameraList.end());
}

#pragma endregion
