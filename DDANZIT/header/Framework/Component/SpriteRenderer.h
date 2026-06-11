#pragma once

#include <string>
#include <unordered_map>

#include "DefineOption.h"
#include "Color.h"

#include "Component.h"

//enum class SpriteDrawMode
//{
//	Simple,
//	Sliced,
//	Tiled,
//};

// 픽셀좌표 직접 알아오세요
struct SpriteAtlasRect
{
	int pixel_OffsetX;
	int pixel_OffsetY;
	int pixel_Width;
	int pixel_Height;
};


// 비트맵 / 텍스처 ...가 있는 곳 정보와 기타등등 가질 것이다
class SpriteRenderer : public Component
{
public:
	friend class DDANZIT_Core;
	friend class Debug;
	friend class Scene;
	friend class GameObject;
	friend class Draw2D;

#pragma region Constructor

private:
	SpriteRenderer() = delete;
	SpriteRenderer(const SpriteRenderer& other) = default;
	SpriteRenderer(GameObject* pGameObject);

public:
	~SpriteRenderer() = default;

#pragma endregion



#pragma region Clone

private:
	Component* Clone() const override;

#pragma endregion



#pragma region Properties

	// 이건 커맨드 할만한데..?
public:
	Sprite sprite;
	Color color;
	bool flipX;
	bool flipY;
	//SpriteDrawMode drawMode;
	bool useAtlas;

public:
	std::unordered_map<std::string, SpriteAtlasRect> atlasRectMap;

	SpriteAtlasRect currentAtlas;

#pragma endregion



#pragma region Methods

public:
	bool TryAddAtlasRect(const std::string& name, SpriteAtlasRect sliceRect);
	bool TryAddAtlasRect(const std::string& name, int offsetX, int offsetY, int width, int height);	// 위에서 아래로

	bool TrySetAtlas(const std::string& name);

#pragma endregion

};

