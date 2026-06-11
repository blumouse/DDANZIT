#pragma once

//#include "DDANZITEngine.h"
#include "MonoBehavior.h"


class Script : public MonoBehavior 
{
public:
	Script(GameObject* gameObject);

	// 인스턴트 하고싶으면 복사생성자 / 클론 함수 재정의도 하셔야 해요

	void Awake();
};