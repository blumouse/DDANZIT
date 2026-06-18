#include "Game/GuardAttack/RhythmGuardAttackObject.h"

#include "DDANZITEngine.h"
#include "Game/GuardAttack/RhythmGuardAttackController.h"

RhythmGuardAttackObject::RhythmGuardAttackObject(Scene* scene) : GameObject(scene), Draw2D(this)
{
	AddComponent<SpriteRenderer>();
	AddComponent<Rigidbody2D>();
	CircleCollider2D* collider = AddComponent<CircleCollider2D>();
	collider->SetRadius(150.0f);
	AddComponent<RhythmGuardAttackController>();
}

GameObject* RhythmGuardAttackObject::Clone() const
{
	return new RhythmGuardAttackObject(*this);
}
