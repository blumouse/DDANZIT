#pragma once

#include "MonoBehavior.h"
#include "Game/Common/RhythmTypes.h"

class Collider2D;
class GameObject;
class PlayerController;

// Component attached to each player action hitbox.
// Enemy and guard attack objects use this data when their trigger event fires.
class PlayerActionHitboxScript : public MonoBehavior
{
public:
	PlayerActionHitboxScript(GameObject* gameObject);

	void OnTriggerEnter2D(Collider2D* collision) override;
	void OnTriggerStay2D(Collider2D* collision) override;

	void Bind(PlayerController* owner, RhythmDirection direction);

	PlayerController* Owner() const;
	RhythmDirection Direction() const;
	RhythmActionKind ActiveActionKind() const;
	bool TryConsume(RhythmActionKind actionKind);

private:
	bool TryResolveTrigger(Collider2D* collision);

	PlayerController* owner = nullptr;
	RhythmDirection direction = RhythmDirection::Left;
};
