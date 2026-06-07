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
	if (_scene != nullptr)
		return SceneManager::GetActiveScene()->backgroundColor();
	else
		return _scene->backgroundColor();
}

const Color& Camera::backgroundColor() const
{
	if (_scene != nullptr)
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



#pragma region IRenderer

void Camera::Render(HDC hdc)
{
	// 카메라 단 오브젝트의 뎁스부터 컬링
#ifdef PROPS_MODE_2D

	float camPosX = transform()->_position.x;
	float camPosY = transform()->_position.y;

	for (int i = MAX_LAYER_NUM; i >= depth(); i--)
	{
		for (const DrawCommand& cmd : DDANZIT_Core::drawCommandLists[i])
		{
#ifdef RENDER_MODE_WINGDI
			if ((int)cmd.spriteIndex < 0)
			{
				switch (cmd.spriteIndex)
				{
				default:
				case SpriteIndex::None:

					continue;
				case SpriteIndex::Sqaure:
					// TODO: 직접 그려서 만들고 캐싱
					break;
				case SpriteIndex::Circle:

					break;
				case SpriteIndex::Capsule:

					break;
				}
			}
			else
			{
				BitmapInfo* bmi = DDANZIT_Core::bitmapResourceList[(int)cmd.spriteIndex];

				if (bmi == nullptr) return;
				if (bmi->GetBitmapHandle() == nullptr) return;

				HDC hBitmapDC = CreateCompatibleDC(hdc);

				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hBitmapDC, bmi->GetBitmapHandle());

				// 카메라 + 화면 중심 좌표로 계산
				// 카메라 벡터는 빼고 화면 중심좌표 계산해서 더해 (y축 뒤집어야되나?)
				// 아 하나더 원본 비트맵 기준으로 배율을 주는거로 계산..

				const float WorldToScreen = 5.0f;
				
				float relativeCamX = cmd.posX - camPosX;
				float relativeCamY = cmd.posY - camPosY;

				BITMAP bmp;
				int relativeScaleX;
				int relativeScaleY;

				if (cmd.useAtlas)
				{
					relativeScaleX = cmd.scaleX * cmd.sliceWidth;
					relativeScaleY = cmd.scaleY * cmd.sliceHeight;
				}
				else
				{
					GetObject(bmi->GetBitmapHandle(), sizeof(BITMAP), &bmp);

					relativeScaleX = cmd.scaleX * bmp.bmWidth;
					relativeScaleY = cmd.scaleY * bmp.bmHeight;
				}

				int x = (int)((relativeCamX - relativeScaleX / 2.0f) * WorldToScreen) + DDANZIT_Core::width / 2;
				int y = (int)((relativeCamX - relativeScaleY / 2.0f) * WorldToScreen) + DDANZIT_Core::height / 2;


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
				blend.SourceConstantAlpha = (cmd.colorRGBA ^ 0x000000ff);
				blend.AlphaFormat = AC_SRC_ALPHA;

				// 회전도 무리


				AlphaBlend(hdc, x, y, relativeScaleX, relativeScaleY,
					hBitmapDC, srcX, srcY, srcWidth, srcHeight, blend);


				SelectObject(hBitmapDC, hOldBitmap);
				DeleteDC(hBitmapDC);
			}

#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D



#endif // RENDER_MODE_DIRECT2D

		}
	}


	// TODO: 콜라이더 선 그리기
	for (int i = MAX_LAYER_NUM; i >= depth(); i--)
	{
		for (const DebugDrawCommand& cmd : DDANZIT_Core::debugDrawCommandLists[i])
		{
#ifdef RENDER_MODE_WINGDI



#endif // RENDER_MODE_WINGDI

#ifdef RENDER_MODE_DIRECT2D



#endif // RENDER_MODE_DIRECT2D
		}
	}

#endif // PROPS_MODE_2D

}

#pragma endregion



#pragma region StaticMethod

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
