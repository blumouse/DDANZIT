#include "Game/UI/HealthBarObserver.h"

#include "DDANZITEngine.h"
#include "Player.h"

#include <algorithm>

HealthBarObserver::HealthBarObserver(GameObject* gameObject) : MonoBehavior(gameObject)
{
	EnableAwake();
}

void HealthBarObserver::Awake()
{
	spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();
}

void HealthBarObserver::SetTarget(PlayerController* targetPlayer)
{
	this->targetPlayer = targetPlayer;

	if (targetPlayer != nullptr)
		targetPlayer->SubscribeHealthChanged(this, &HealthBarObserver::OnHealthChanged);

	ResetView();
}

void HealthBarObserver::ResetView()
{
	if (!hasBaseScale)
	{
		baseScale = transform()->scale();
		hasBaseScale = true;
	}

	ApplyHealth(cachedCurrentHealth, cachedMaxHealth);
}

void HealthBarObserver::OnHealthChanged(void* context, int currentHealth, int maxHealth)
{
	HealthBarObserver* observer = static_cast<HealthBarObserver*>(context);
	if (observer != nullptr)
		observer->ApplyHealth(currentHealth, maxHealth);
}

void HealthBarObserver::ApplyHealth(int currentHealth, int maxHealth)
{
	cachedCurrentHealth = currentHealth;
	cachedMaxHealth = std::max(1, maxHealth);

	if (!hasBaseScale)
	{
		baseScale = transform()->scale();
		hasBaseScale = true;
	}

	const float ratio = std::clamp(static_cast<float>(cachedCurrentHealth) / static_cast<float>(cachedMaxHealth), 0.0f, 1.0f);
	transform()->SetScale(Vector2(baseScale.x * ratio, baseScale.y));

	if (spriteRenderer == nullptr)
		spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();

	if (spriteRenderer == nullptr)
		return;

	if (ratio > 0.55f)
		spriteRenderer->color = Color(0.25f, 0.95f, 0.35f, 1.0f);
	else if (ratio > 0.25f)
		spriteRenderer->color = Color(1.0f, 0.85f, 0.20f, 1.0f);
	else
		spriteRenderer->color = Color(1.0f, 0.25f, 0.18f, 1.0f);
}
