#pragma once

#include "DefineOption.h"
#include "Component.h"

typedef SpriteIndex Sprite;

struct Color 
{
	float a;
	float r;
	float g;
	float b;
};

//enum class SpriteDrawMode
//{
//	Simple,
//	Sliced,
//	Tiled,
//};


// TODO: 기본 컴포넌트들 초기값 어케 넣어줄까?
// 비트맵 / 텍스처 ...가 있는 곳 정보와 기타등등 가질 것이다
class SpriteRenderer : public Component
{
public:
	friend class DDANZIT_Core;
	friend class Scene;
	friend class GameObject;

#pragma region Constructor

private:
	SpriteRenderer() = default;
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

#pragma endregion



#pragma region Methods

#pragma endregion

};

