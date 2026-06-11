#include "characterAnimation.h"

#include "DDANZITEngine.h"

characterAnimation::characterAnimation(GameObject* gameObject) : MonoBehavior(gameObject)
{
	EnableAwake();
	EnableUpdate();
}

// 인스턴트 하고싶으면 복사생성자 / 클론 함수 재정의도 하셔야 해요

void characterAnimation::Awake()
{
	auto comp = gameObject()->GetComponent<characterAnimation>();
	spRender = gameObject()->GetComponent<SpriteRenderer>();
}

//애니메이션별 상태 나누기
//스프라이트 이미지 크기 맞추기

void characterAnimation::Update() 
{	
	elapsedTime += Time::deltaTime();

	if (elapsedTime >= 100.f/*frameDuration */ )
	{
		elapsedTime = 0.0f;
		spRender->currentAtlas.pixel_OffsetX++;
		if (spRender->currentAtlas.pixel_OffsetX % 8 == 0) {
			spRender->currentAtlas.pixel_OffsetY++;
			spRender->currentAtlas.pixel_OffsetX = 0;
		}
		//임시
		if (spRender->currentAtlas.pixel_OffsetY >= 3) {
			spRender->currentAtlas.pixel_OffsetX = 0;
			spRender->currentAtlas.pixel_OffsetY = 0;
		}
	}
}

void characterAnimation::SetDuration(float time) {
	frameDuration = time;
}