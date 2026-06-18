#pragma once

#include "Draw2D.h"
#include "GameObject.h"

// UiBoxObject는 메뉴 카드, 진행바, 판정 지점 같은 단순 시각 요소입니다.
// 텍스트 렌더링에 의존하지 않고도 게임 상태를 화면에 보여주기 위해 씁니다.
class UiBoxObject : public GameObject, public Draw2D
{
public:
	UiBoxObject(Scene* scene);
	UiBoxObject(const UiBoxObject& other) = default;

	GameObject* Clone() const override;
};
