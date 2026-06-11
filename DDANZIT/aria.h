#pragma once

//#include "DDANZITEngine.h"
#include "GameObject.h"
#include "Draw2D.h"
#include "Script.h"


//궁우 애니메이션 관리를 위한 클래스
class aria : public GameObject, public Draw2D
{
public:
	aria(Scene* scene);

	aria(const aria& other) = default;

	GameObject* Clone() const override;

	~aria() = default;
};
