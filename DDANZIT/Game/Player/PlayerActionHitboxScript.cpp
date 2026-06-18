#include "Game/Player/PlayerActionHitboxScript.h"

#include "DDANZITEngine.h"
#include "Game/Enemy/RhythmEnemyController.h"
#include "Game/GuardAttack/RhythmGuardAttackController.h"
#include "Player.h"

PlayerActionHitboxScript::PlayerActionHitboxScript(GameObject* gameObject) : MonoBehavior(gameObject)
{
	EnableOnTriggerEnter2D();
	EnableOnTriggerStay2D();
}

void PlayerActionHitboxScript::OnTriggerEnter2D(Collider2D* collision)
{
	TryResolveTrigger(collision);
}

void PlayerActionHitboxScript::OnTriggerStay2D(Collider2D* collision)
{
	TryResolveTrigger(collision);
}

void PlayerActionHitboxScript::Bind(PlayerController* owner, RhythmDirection direction)
{
	this->owner = owner;
	this->direction = direction;
}

PlayerController* PlayerActionHitboxScript::Owner() const
{
	return owner;
}

RhythmDirection PlayerActionHitboxScript::Direction() const
{
	return direction;
}

RhythmActionKind PlayerActionHitboxScript::ActiveActionKind() const
{
	if (owner == nullptr)
		return RhythmActionKind::Fire;

	return owner->CurrentActionKind(direction);
}

bool PlayerActionHitboxScript::TryConsume(RhythmActionKind actionKind)
{
	return owner != nullptr && owner->TryConsumeActionFromTrigger(direction, actionKind);
}

bool PlayerActionHitboxScript::TryResolveTrigger(Collider2D* collision)
{
	if (collision == nullptr || collision->gameObject() == nullptr)
		return false;

	RhythmEnemyController* enemy = nullptr;
	if (collision->gameObject()->TryGetComponent(enemy))
		return enemy->TryResolvePlayerAction(this);

	RhythmGuardAttackController* guardAttack = nullptr;
	if (collision->gameObject()->TryGetComponent(guardAttack))
		return guardAttack->TryResolvePlayerAction(this);

	return false;
}
