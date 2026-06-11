#pragma once

//#include "DDANZITEngine.h"
#include "GameObject.h"
#include "Draw2D.h"
#include "Script.h"


//궁우 애니메이션 관리를 위한 클래스
class gungwoo : public GameObject, public Draw2D
{
public:
	gungwoo(Scene* scene);

	gungwoo(const gungwoo& other) = default;

	GameObject* Clone() const override;

	~gungwoo() = default;



};
