#include "Camera.h"

#include "Debug.h"

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
	Component(pGameObject, "Camera")
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

void Camera::Render(ID2D1DeviceContext4* d2dcontext, ID2D1SolidColorBrush* d2dbrush, ID2D1Effect* colorMatrixEffect, IDWriteFactory5* writeFactory)
{
	// 카메라 단 오브젝트의 뎁스부터 컬링
#ifdef PROPS_MODE_2D

	Vector2 camPos = transform()->position();
	float camAngle = transform()->angle();

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

				float relativeScaleX = cmd.scaleX;
				float relativeScaleY = cmd.scaleY;

				float relativeAngle = cmd.angle - camAngle;


				D2D1_POINT_2F centerPos = D2D1::Point2F(0.0f, 0.0f);

				D2D1_MATRIX_3X2_F scaleMatrix = D2D1::Matrix3x2F::Identity();
				D2D1_MATRIX_3X2_F rotMatrix = D2D1::Matrix3x2F::Identity();
				D2D1_MATRIX_3X2_F transMatrix = D2D1::Matrix3x2F::Identity();


				scaleMatrix = D2D1::Matrix3x2F::Scale(relativeScaleX, relativeScaleY, centerPos);
				// 플립은 하나마나니까 생략;

				rotMatrix = D2D1::Matrix3x2F::Rotation(relativeAngle, centerPos);

				transMatrix = D2D1::Matrix3x2F::Translation(
					relativeCamX + (float)DDANZIT_Core::width / 2.0f,		// 월드 (0,0)기준으로
					-relativeCamY + (float)DDANZIT_Core::height / 2.0f);

				d2dcontext->SetTransform(scaleMatrix * rotMatrix * transMatrix);


				UINT32 rgb;
				float a;
				rgb = (cmd.colorRGBA >> 8) & 0x00ffffff;
				a = (float)(cmd.colorRGBA & 0x000000ff) / 255.0f;

				d2dbrush->SetColor(D2D1::ColorF(rgb, a));


				switch (cmd.spriteIndex)
				{
				default:	// ?
					continue;

				case SpriteIndex::Sqaure:

					d2dcontext->FillRectangle(D2D1::RectF(-50.0f, -50.0f, 50.0f, 50.0f), d2dbrush);

					break;

				case SpriteIndex::Circle:

					d2dcontext->FillEllipse(D2D1::Ellipse(centerPos, 50.0f, 50.0f), d2dbrush);

					break;

				case SpriteIndex::Capsule:

					d2dcontext->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(-50.0f, -100.0f, 50.0f, 100.0f), 50.0f, 50.0f), d2dbrush);

					break;
				}


				d2dcontext->SetTransform(D2D1::Matrix3x2F::Identity());

			}
			else
			{
				ID2D1Bitmap* pBitmap = DDANZIT_Core::bitmapResourceList[(int)cmd.spriteIndex].Get();

				if (pBitmap == nullptr)
				{
					Debug::Log("Render: 비트맵 설정이 잘못되었습니다.");
					continue;
				}


				D2D1_MATRIX_3X2_F scaleFlipMatrix = D2D1::Matrix3x2F::Identity();
				D2D1_MATRIX_3X2_F rotMatrix = D2D1::Matrix3x2F::Identity();
				D2D1_MATRIX_3X2_F transMatrix = D2D1::Matrix3x2F::Identity();


				float relativeCamX = cmd.posX - camPosX;
				float relativeCamY = cmd.posY - camPosY;

				D2D1_POINT_2F centerPos = D2D1::Point2F(0.0f, 0.0f);

				int pixelWidth = pBitmap->GetPixelSize().width;
				int pixelHeight = pBitmap->GetPixelSize().height;

				// 크기
				float relativeScaleX;
				float relativeScaleY;

				//if (cmd.useAtlas)
				//{
				//	relativeScaleX = (float)cmd.sliceWidth * cmd.scaleX / worldToScreenRatio;	// 비트맵 픽셀좌표를 월드로 밀어넣는다
				//	relativeScaleY = (float)cmd.sliceHeight * cmd.scaleY / worldToScreenRatio;
				//}
				//else
				//{
				//	relativeScaleX = cmd.scaleX / worldToScreenRatio;
				//	relativeScaleY = cmd.scaleY / worldToScreenRatio;
				//}

				relativeScaleX = cmd.scaleX / worldToScreenRatio;
				relativeScaleY = cmd.scaleY / worldToScreenRatio;

				float relativeAngle = cmd.angle - camAngle;


				// 반전이랑 스케일링이랑 동치라고 하네요
				if (cmd.flipX)
				{
					if (cmd.flipY)
						scaleFlipMatrix = D2D1::Matrix3x2F::Scale(-relativeScaleX, -relativeScaleY, centerPos);
					else
						scaleFlipMatrix = D2D1::Matrix3x2F::Scale(-relativeScaleX, relativeScaleY, centerPos);
				}
				else if (cmd.flipY)
				{
					scaleFlipMatrix = D2D1::Matrix3x2F::Scale(relativeScaleX, -relativeScaleY, centerPos);
				}
				else
				{
					scaleFlipMatrix = D2D1::Matrix3x2F::Scale(relativeScaleX, relativeScaleY, centerPos);
				}

				// 회전
				rotMatrix = D2D1::Matrix3x2F::Rotation(relativeAngle, centerPos);

				// 이동
				transMatrix = D2D1::Matrix3x2F::Translation(
					relativeCamX + (float)DDANZIT_Core::width / 2.0f,		// 월드 (0,0)기준으로
					-relativeCamY + (float)DDANZIT_Core::height / 2.0f);

				// 적용!
				d2dcontext->SetTransform(scaleFlipMatrix * rotMatrix * transMatrix);


				// 색상 곱하기!
				float r;	float g;	float b;	float a;
				r = (float)((cmd.colorRGBA >> 24) & 0x000000ff) / 255.0f;
				g = (float)((cmd.colorRGBA >> 16) & 0x000000ff) / 255.0f;
				b = (float)((cmd.colorRGBA >> 8) & 0x000000ff) / 255.0f;
				a = (float)(cmd.colorRGBA & 0x000000ff) / 255.0f;

				D2D1_MATRIX_5X4_F colorMatrix = D2D1::Matrix5x4F(
					r, 0.0f, 0.0f, 0.0f,
					0.0f, g, 0.0f, 0.0f,
					0.0f, 0.0f, b, 0.0f,
					0.0f, 0.0f, 0.0f, a,
					0.0f, 0.0f, 0.0f, 0.0f   // Offset (더하기 값)
				);


				// 비트맵 자르기
				int srcX;	int srcWidth;
				int srcY;	int srcHeight;

				if (cmd.useAtlas)
				{
					srcX = cmd.sliceOffsetX;
					srcY = cmd.sliceOffsetY;

					srcWidth = cmd.sliceWidth;
					srcHeight = cmd.sliceHeight;
				}
				else
				{
					srcX = 0;
					srcY = 0;

					srcWidth = pixelWidth;
					srcHeight = pixelHeight;
				}


				// 그리기~
				D2D1_POINT_2F localOffset = D2D1::Point2F(srcWidth / -2.0f - srcX, srcHeight / -2.0f - srcY);
				//D2D1_RECT_F destRect = D2D1::RectF(x, y, x + relativeScaleX * worldToScreenRatio, y + relativeScaleY * worldToScreenRatio);
				D2D1_RECT_F srcRect = D2D1::RectF(srcX, srcY, srcWidth, srcHeight);


				colorMatrixEffect->SetInput(0, pBitmap);

				colorMatrixEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, colorMatrix);


				//d2dcontext->DrawBitmap(
				//	pBitmap,
				//	&destRect,
				//	a,
				//	D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				//	&srcRect);

				d2dcontext->DrawImage(
					colorMatrixEffect,
					&localOffset,
					&srcRect,
					D2D1_INTERPOLATION_MODE_LINEAR);


				d2dcontext->SetTransform(D2D1::Matrix3x2F::Identity());
			}

		}
	}


#ifdef USE_DEBUG
	for (int i = MAX_LAYER_NUM - 1; i >= depth(); i--)
	{
		for (const DebugDrawCommand& cmd : DDANZIT_Core::debugDrawCommandLists[i])
		{
			float relativeCamX = cmd.posX - camPosX;
			float relativeCamY = cmd.posY - camPosY;

			float relativeScaleX = cmd.scaleX;
			float relativeScaleY = cmd.scaleY;

			float relativeAngle = cmd.angle - camAngle;


			D2D1_POINT_2F centerPos = D2D1::Point2F(0.0f, 0.0f);

			D2D1_MATRIX_3X2_F scaleMatrix = D2D1::Matrix3x2F::Identity();
			D2D1_MATRIX_3X2_F rotMatrix = D2D1::Matrix3x2F::Identity();
			D2D1_MATRIX_3X2_F transMatrix = D2D1::Matrix3x2F::Identity();

			if (cmd.debugDrawType == DebugDrawType::CircleCollider)
				scaleMatrix = D2D1::Matrix3x2F::Scale(relativeScaleX, relativeScaleX, centerPos);
			else
				scaleMatrix = D2D1::Matrix3x2F::Scale(relativeScaleX, relativeScaleY, centerPos);
			// 플립은 하나마나니까 생략;

			rotMatrix = D2D1::Matrix3x2F::Rotation(relativeAngle, centerPos);

			transMatrix = D2D1::Matrix3x2F::Translation(
				relativeCamX + (float)DDANZIT_Core::width / 2.0f,		// 월드 (0,0)기준으로
				-relativeCamY + (float)DDANZIT_Core::height / 2.0f);

			d2dcontext->SetTransform(scaleMatrix * rotMatrix * transMatrix);


			UINT32 rgb;
			float a;
			rgb = (cmd.colorRGBA >> 8) & 0x00ffffff;
			a = (float)(cmd.colorRGBA & 0x000000ff) / 255.0f;

			d2dbrush->SetColor(D2D1::ColorF(rgb, a));


			switch (cmd.debugDrawType)
			{
			case DebugDrawType::BoxCollider:

				d2dcontext->DrawRectangle(D2D1::RectF(-50.0f, -50.0f, 50.0f, 50.0f), d2dbrush);

				break;

			case DebugDrawType::CircleCollider:

				d2dcontext->DrawEllipse(D2D1::Ellipse(centerPos, 50.0f, 50.0f), d2dbrush);

				break;

			case DebugDrawType::CapsuleCollider:

				d2dcontext->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(-50.0f, -100.0f, 50.0f, 100.0f), 50.0f, 50.0f), d2dbrush);

				break;
			}


			d2dcontext->SetTransform(D2D1::Matrix3x2F::Identity());

		}
	}

#endif // USE_DEBUG


#endif // PROPS_MODE_2D


	// UI는 언제나 화면 가장 위 카메라 뎁스같은거 업서

	for (int i = MAX_LAYER_NUM - 1; i >= 0; i--)
	{
		for (const UIDrawCommand& cmd : DDANZIT_Core::UIDrawCommandLists[i])
		{
			if (cmd.uiDrawType == UIDrawType::Image)
			{
				// TODO 아직 미지원... 이지만 뭐 스프라이트 그리는거랑 같다는거
			}
			else if (cmd.uiDrawType == UIDrawType::Text)
			{
				IDWriteTextFormat* pFont = DDANZIT_Core::fontResourceList[(int)cmd.fontIndex][cmd.fontSizeIndex].Get();

				if (pFont == nullptr)
				{
					Debug::Log("Render: 폰트 설정이 잘못되었습니다.");
					continue;
				}

				IDWriteTextLayout* pTextLayout = nullptr;

				writeFactory->CreateTextLayout(
					cmd.text.data(),
					cmd.text.length(),
					pFont,
					cmd.scaleX,
					cmd.scaleY,
					&pTextLayout
				);

				if (pTextLayout == nullptr)
				{
					Debug::Log("Render: 텍스트 생성에 실패했습니다.");
					continue;
				}


				pTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				pTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

				// TODO 기본 정렬 / 오버플로우 / 행간 자간 설정


				D2D1_MATRIX_3X2_F scaleFlipMatrix = D2D1::Matrix3x2F::Identity();
				D2D1_MATRIX_3X2_F rotMatrix = D2D1::Matrix3x2F::Identity();
				D2D1_MATRIX_3X2_F transMatrix = D2D1::Matrix3x2F::Identity();


				D2D1_POINT_2F centerPos = D2D1::Point2F(0.0f, 0.0f);


				if (cmd.flipX)
				{
					if (cmd.flipY)
						scaleFlipMatrix = D2D1::Matrix3x2F::Scale(-1.0f, -1.0f, centerPos);
					else
						scaleFlipMatrix = D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, centerPos);
				}
				else if (cmd.flipY)
				{
					scaleFlipMatrix = D2D1::Matrix3x2F::Scale(1.0f, -1.0f, centerPos);
				}
				else
				{
					scaleFlipMatrix = D2D1::Matrix3x2F::Scale(1.0f, 1.0f, centerPos);
				}

				// 회전
				rotMatrix = D2D1::Matrix3x2F::Rotation(cmd.angle, centerPos);

				// 이동
				transMatrix = D2D1::Matrix3x2F::Translation(
					cmd.posX + (float)DDANZIT_Core::width / 2.0f,		// 월드 (0,0)기준으로
					-cmd.posY + (float)DDANZIT_Core::height / 2.0f);

				// 적용!
				d2dcontext->SetTransform(scaleFlipMatrix * rotMatrix * transMatrix);


				UINT32 rgb;
				float a;
				rgb = (cmd.colorRGBA >> 8) & 0x00ffffff;
				a = (float)(cmd.colorRGBA & 0x000000ff) / 255.0f;

				d2dbrush->SetColor(D2D1::ColorF(rgb, a));


				// 그리기~
				D2D1_POINT_2F localOffset = D2D1::Point2F(cmd.scaleX / -2.0f, cmd.scaleY / -2.0f);

				d2dcontext->DrawTextLayout(
					localOffset,
					pTextLayout,
					d2dbrush
				);

				pTextLayout->Release();

				d2dcontext->SetTransform(D2D1::Matrix3x2F::Identity());
			}
		}
	}


}

#endif // RENDER_MODE_DIRECT2D


#pragma endregion



#pragma region StaticMethod

const float Camera::worldToScreenRatio = 5.0f;

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
		Debug::Log("SetupCurrent: 컴포넌트가 nullptr입니다.");
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
