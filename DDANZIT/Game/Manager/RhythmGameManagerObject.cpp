#include "Game/Manager/RhythmGameManagerObject.h"

#include "DDANZITEngine.h"
#include "Game/Manager/RhythmGameManager.h"

RhythmGameManagerObject::RhythmGameManagerObject(Scene* scene) : GameObject(scene)
{
	AddComponent<RhythmGameManager>();
}

GameObject* RhythmGameManagerObject::Clone() const
{
	return new RhythmGameManagerObject(*this);
}
