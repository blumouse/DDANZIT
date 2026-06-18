#include "Game/Enemy/RhythmEnemyObject.h"

#include "DDANZITEngine.h"
#include "Game/Enemy/RhythmEnemyController.h"

RhythmEnemyObject::RhythmEnemyObject(Scene* scene) : GameObject(scene), Draw2D(this)
{
	AddComponent<SpriteRenderer>();
	AddComponent<Rigidbody2D>();
	CircleCollider2D* collider = AddComponent<CircleCollider2D>();
	collider->SetRadius(140.0f);
	AddComponent<RhythmEnemyController>();
}

GameObject* RhythmEnemyObject::Clone() const
{
	return new RhythmEnemyObject(*this);
}
