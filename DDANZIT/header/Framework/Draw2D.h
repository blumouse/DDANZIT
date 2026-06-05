#pragma once

#include "DefineOption.h"
#include "IDrawable.h"

class GameObject;
class SpriteRenderer;


#ifdef PROPS_MODE_2D

// 오브젝트에 붙이세요
class Draw2D : public IDrawable
{

#pragma region Constructor

protected:
	Draw2D() = default;
	Draw2D(const Draw2D&) = default;
	Draw2D(GameObject* gameObject);

public:
	virtual ~Draw2D() = default;

#pragma endregion



#pragma region Properties

private:
	int layer = 0;
	GameObject* gameObject;

	SpriteRenderer* spriteRenderer;

#pragma endregion



#pragma region IDrawable

	void Draw(HDC hdc) override;

	// depth랑 연동해서 로직타임에 바꿔놓기로 하자
	int GetLayer() override;
	void SetLayer(int layer) override;

#pragma endregion

};

#endif // PROPS_MODE_2D

