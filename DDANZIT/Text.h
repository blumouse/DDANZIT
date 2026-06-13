#pragma once

#include <vector>
#include <string>

#include "DefineOption.h"
#include "Color.h"

#include "DDANZIT_Core.h"

#include "Component.h"

enum class FontSize
{
	_12, 
	_14, 
	_16, 
	_18, 
	_20, 
	_24, 
	_28, 
	_32, 
	_36, 
	_40, 
	_48, 
	_60,
};


class Text : public Component
{

#pragma region Constructor

public:
	Text() = delete;
	Text(const Text& other) = default;
	Text(GameObject* pGameObject);

public:
	~Text() = default;

#pragma endregion



#pragma region Clone

private:
	Component* Clone() const override;

#pragma endregion



#pragma region Properties

public:
	std::wstring text;
	Font font;
	FontSize fontSize;
	Color color;

	bool flipX;
	bool flipY;

#pragma endregion



#pragma region Methods



#pragma endregion

};

