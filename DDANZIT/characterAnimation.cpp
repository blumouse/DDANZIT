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
    elapsedTime += Time::unscaledDeltaTime();

    if (spRender == nullptr)
        return;

    const float frameStep =
        frameDuration / static_cast<float>(27);

    while (elapsedTime >= frameStep)
    {
        elapsedTime -= frameStep;

        spRender->currentAtlas.pixel_OffsetX++;

        if (spRender->currentAtlas.pixel_OffsetX >= 9)
        {
            spRender->currentAtlas.pixel_OffsetX = 0;
            spRender->currentAtlas.pixel_OffsetY++;
        }

        if (spRender->currentAtlas.pixel_OffsetY >= 3)
        {
            spRender->currentAtlas.pixel_OffsetX = 0;
            spRender->currentAtlas.pixel_OffsetY = 0;
        }
    }
}

void characterAnimation::SetDuration(float time) {
	frameDuration = time;
}
