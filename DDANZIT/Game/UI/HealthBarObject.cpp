#include "Game/UI/HealthBarObject.h"

#include "DDANZITEngine.h"
#include "Game/UI/HealthBarObserver.h"

HealthBarObject::HealthBarObject(Scene* scene) : GameObject(scene), Draw2D(this)
{
	AddComponent<SpriteRenderer>();
	AddComponent<HealthBarObserver>();
}

GameObject* HealthBarObject::Clone() const
{
	return new HealthBarObject(*this);
}
