#pragma once
#include "GameObject.h"

// 사용자정의 확장용 때묻지 않은 순수한 컴포넌트..
class Component
{
public:
	GameObject* GetGameObject() { return gameObject; }

protected:
	GameObject* gameObject;
};

