#include "Game/Effect/RhythmEffectObject.h"

#include "DDANZITEngine.h"
#include "Game/Effect/RhythmEffectController.h"

RhythmEffectObject::RhythmEffectObject(Scene* scene) : GameObject(scene), Draw2D(this)
{
	AddComponent<SpriteRenderer>();
	AddComponent<RhythmEffectController>();
}

GameObject* RhythmEffectObject::Clone() const
{
	return new RhythmEffectObject(*this);
}
