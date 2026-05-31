#include "Transform.h"
#include "Game.h"

Transform::Transform() : pos(Vector2(0,0)), scale(Vector2{ 1,1 })
{
	Game::GetInstance()->RegisterTransform(this);
}


bool Transform::IsIntersectPoint(int x, int y)
{
	if (pos.x - (scale.x / 2.0f) < x && x < pos.x + (scale.x / 2.0f))
	{
		if (pos.y - (scale.y / 2.0f) < y && y < pos.y + (scale.y / 2.0f))
			return true;
	}

	return false;
}