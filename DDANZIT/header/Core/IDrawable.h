#pragma once


// 이것의 구현체를 하나 더 만들어서 그걸 상속주는걸로 하자
class IDrawable 
{
protected:
	IDrawable() = default;

public:
	IDrawable(const IDrawable&) = default;
	virtual ~IDrawable() = default;

	virtual void Draw() = 0;

	// depth랑 연동해서 로직타임에 바꿔놓기로 하자
	virtual int GetLayer() = 0;
	virtual void SetLayer(int layer) = 0;
};