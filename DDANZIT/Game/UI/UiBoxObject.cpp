#include "Game/UI/UiBoxObject.h"

#include "DDANZITEngine.h"

UiBoxObject::UiBoxObject(Scene* scene) : GameObject(scene), Draw2D(this)
{
	AddComponent<SpriteRenderer>();
}

GameObject* UiBoxObject::Clone() const
{
	return new UiBoxObject(*this);
}
