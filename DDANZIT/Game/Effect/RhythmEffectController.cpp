#include "Game/Effect/RhythmEffectController.h"

#include "DDANZITEngine.h"

#include <algorithm>
#include <cmath>

namespace
{
	Color WithAlpha(const Color& color, float alpha)
	{
		return Color(color.r, color.g, color.b, alpha);
	}

	Vector2 LerpVector(const Vector2& from, const Vector2& to, float t)
	{
		return from + (to - from) * t;
	}
}

RhythmEffectController::RhythmEffectController(GameObject* gameObject) : MonoBehavior(gameObject)
{
	EnableAwake();
}

void RhythmEffectController::Awake()
{
	spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();
	Deactivate();
}

void RhythmEffectController::InitializePoolSlot(int slotIndex)
{
	this->slotIndex = slotIndex;
	Deactivate();
}

void RhythmEffectController::Activate(
	RhythmEffectType type,
	SpriteIndex sprite,
	Vector2 position,
	Vector2 scale,
	Color color,
	float lifeSeconds,
	Vector2 velocity)
{
	this->type = type;
	this->velocity = velocity;
	this->baseScale = scale;
	this->baseColor = color;
	this->lifeSeconds = std::max(0.01f, lifeSeconds);
	ageSeconds = 0.0f;
	activeEffect = true;

	transform()->SetPosition(position);
	transform()->SetScale(scale);

	if (spriteRenderer == nullptr)
		spriteRenderer = gameObject()->GetComponent<SpriteRenderer>();

	if (spriteRenderer != nullptr)
	{
		spriteRenderer->useAtlas = false;
		spriteRenderer->sprite = sprite;
		spriteRenderer->flipX = false;
		spriteRenderer->flipY = false;
		spriteRenderer->color = color;
	}

	gameObject()->SetActive(true);
}

void RhythmEffectController::Deactivate()
{
	activeEffect = false;
	gameObject()->SetActive(false);
}

void RhythmEffectController::Tick(float deltaSeconds)
{
	if (!activeEffect)
		return;

	ageSeconds += deltaSeconds;
	const float progress = std::clamp(ageSeconds / std::max(0.01f, lifeSeconds), 0.0f, 1.0f);

	if (type == RhythmEffectType::Light)
	{
		if (spriteRenderer != nullptr)
			spriteRenderer->color = WithAlpha(baseColor, baseColor.a * (1.0f - progress));
	}
	else
	{
		transform()->SetPosition(transform()->position() + velocity * deltaSeconds);
		transform()->SetScale(transform()->scale() * std::exp(-6.0f * deltaSeconds));
		velocity = LerpVector(velocity, Vector2(0.0f, 0.0f), std::clamp(9.0f * deltaSeconds, 0.0f, 1.0f));

		if (spriteRenderer != nullptr)
			spriteRenderer->color = WithAlpha(baseColor, 1.0f - progress);
	}

	if (progress >= 1.0f)
		Deactivate();
}

bool RhythmEffectController::IsActiveEffect() const
{
	return activeEffect;
}
