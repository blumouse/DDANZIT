#include "Game/UI/UiTextObject.h"

#include "DDANZITEngine.h"
#include "Text.h"

UiTextObject::UiTextObject(Scene* scene) : GameObject(scene), Draw2D(this)
{
	SpriteRenderer* hiddenRenderer = AddComponent<SpriteRenderer>();
	if (hiddenRenderer != nullptr)
		hiddenRenderer->SetActive(false);

	AddComponent<Text>();
}

GameObject* UiTextObject::Clone() const
{
	return new UiTextObject(*this);
}
