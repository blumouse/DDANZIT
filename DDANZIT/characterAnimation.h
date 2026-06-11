#pragma once

//#include "DDANZITEngine.h"
#include "MonoBehavior.h"

class SpriteRenderer;

class characterAnimation : public MonoBehavior
{
public:
	characterAnimation(GameObject* gameObject);

	// 인스턴트 하고싶으면 복사생성자 / 클론 함수 재정의도 하셔야 해요
	SpriteRenderer* spRender;
	float elapsedTime = 0;
	float frameDuration = 0;

	void Awake();
	void Update();
	void SetDuration(float time);
};