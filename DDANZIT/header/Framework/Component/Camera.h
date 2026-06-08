#pragma once

#include <vector>

#include "DDANZIT_Core.h"

#include "Component.h"
#include "IRenderer.h"

class Scene;
struct Color;


class Camera : public Component, public IRenderer
{
public:
	friend class DDANZIT_Core;
	friend class GameObject;

#pragma region Constructor

private:
	Camera() = delete;

public:
	Camera(const Camera& other) = default;
	Camera(GameObject* pGameObject);

	~Camera();

#pragma endregion



#pragma region Clone

private:
	Component* Clone() const;

#pragma endregion



#pragma region Properties

private:
	Scene* _scene;
public:
	Scene*& scene() { return _scene; }


public:
	int pixelWidth() const { return DDANZIT_Core::width; }
	int pixelHeight() const { return DDANZIT_Core::height; }
	float aspect() const { return (float)DDANZIT_Core::width / (float)DDANZIT_Core::height; }


public:
	Color& backgroundColor();
	const Color& backgroundColor() const;


public:
	void SetDepth(int depth);
	int depth();


#ifdef PROPS_MODE_3D

private:
	float _farClipPlane;
	float _nearClipPlane;
public:
	float& farClipPlane();
	const float& farClipPlane() const;
	float& nearClipPlane();
	const float& nearClipPlane() const;


private:
	bool _orthographic;
public:
	bool& orthographic();
	const bool& orthographic() const;

	// 등등

#endif // PROPS_MODE_3D

#pragma endregion



#pragma region IRenderer

private:
#ifdef RENDER_MODE_WINGDI
	void Render(HDC hdc) override;

#endif

#ifdef RENDER_MODE_DIRECT2D
	void Render(ID2D1DeviceContext4* d2dcontext, ID2D1SolidColorBrush* d2dbrush, ID2D1Bitmap1* d2dtargetBitmap) override;

#endif

#pragma endregion



#pragma region StaticMethod

#ifdef PROPS_MODE_2D

public:
	static const float worldToScreenRatio;

#endif // PROPS_MODE_2D


private:
	static Camera* currentCamera;
	static std::vector<Camera*> allCameraList;

public:
	static Camera* current();
	static void SetupCurrent(Camera* cur);

	static std::vector<Camera*> allCameras();
	static int allCamerasCount();

	// 내부용
private:
	static void RegisterCameraList(Camera* cam);
	static void QuitCameraList(Camera* cam);

#pragma endregion

};

