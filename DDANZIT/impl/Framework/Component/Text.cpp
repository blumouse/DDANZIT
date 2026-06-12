#include "Text.h"


#pragma region Constructor

Text::Text(GameObject* pGameObject) : 
	Component(pGameObject, "Text"), text(L""), font(Font::None), fontSize(FontSize::_20), color(Color(1.0f, 1.0f, 1.0f, 1.0f)), 
	flipX(false), flipY(false)
{
}

#pragma endregion


Component* Text::Clone() const
{
	return new Text(*this);
}

