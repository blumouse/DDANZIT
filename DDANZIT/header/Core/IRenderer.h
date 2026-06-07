#pragma once

typedef struct HDC__* HDC;


class IRenderer
{
protected:
	IRenderer() = default;

public:
	IRenderer(const IRenderer& other) = default;
	virtual ~IRenderer() = default;

	virtual void Render(HDC hdc) = 0;
};