#pragma once

#include "GameObject.h"

// RhythmGameManagerObject는 매니저 컴포넌트를 담기 위한 씬 오브젝트입니다.
// Main.cpp에서 마지막에 생성해 다른 오브젝트들이 먼저 준비된 뒤 초기화되게 합니다.
class RhythmGameManagerObject : public GameObject
{
public:
	RhythmGameManagerObject(Scene* scene);
	RhythmGameManagerObject(const RhythmGameManagerObject& other) = default;

	GameObject* Clone() const override;
};
