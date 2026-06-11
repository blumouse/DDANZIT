#include "Player.h"

#include "DDANZITEngine.h"


Player::Player(Scene* scene) : Draw2D(this)
{
	//사용할 컴포넌트 추가
	AddComponent<Script>();
}

GameObject* Player::Clone() const
{
	return new Player(*this);
}