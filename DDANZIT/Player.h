#pragma once

//#include "DDANZITEngine.h"
#include "GameObject.h"
#include "Draw2D.h"
#include "Script.h"

class Player : public GameObject, public Draw2D
{
public:
	Player(Scene* scene);

	Player(const Player& other) = default;

	GameObject* Clone() const override;

	~Player() = default;



};
