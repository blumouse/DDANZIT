#pragma once

//#include "DDANZITEngine.h"
#include "GameObject.h"
#include "Draw2D.h"
#include "Script.h"


//궁우 애니메이션 관리를 위한 클래스
class suna : public GameObject, public Draw2D
{
public:
	suna(Scene* scene);

	suna(const suna& other) = default;

	GameObject* Clone() const override;

	~suna() = default;
};
