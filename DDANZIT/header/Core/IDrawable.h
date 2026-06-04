#pragma once

#include "INC_Windows.h"

// 이것의 구현체를 하나 더 만들어서 그걸 상속주는걸로 하자 (후일)
class IDrawable {
protected:
	IDrawable() = default;
	IDrawable(const IDrawable&) = delete;

public:
	virtual ~IDrawable() = default;

	virtual void Draw(HDC hdc) = 0;
	virtual int GetLayer() = 0;
	virtual void SetLayer(int layer) = 0;
};