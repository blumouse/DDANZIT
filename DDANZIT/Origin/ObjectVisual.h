#pragma once
#include "Pipeline.h"
#include "Component.h"

class ObjectVisual : public Component, public Pipeline
{
// 파이프라인을 상속할지도 다시 생각해봐야될듯
// 차피 이것들 게임오브젝트쪽에서 갖다쓰는걸
public:
	void Init() override;
	void Update() override;
	void LateUpdate() override;
	void Close() override;

protected:
	// 여기다가 유저 갖다쓰는 재활용성 코드들 두면 될듯
	// 뭔가 채워놓긴 해야되는데 뭐놓지?
};

// 어케 활용해야되지...??
// 여기도 코드를 두지 뭐 아니 머리말고 여기만 조작가능하게 오히려