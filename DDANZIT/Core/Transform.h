#pragma once

#include "Utillity.h"

// 컴포넌트로 넣기엔 뭔가 영 애매하다
// 아니 사실 걍 고정 컴포넌트로 넣는게 맞긴 하겠지만..
using Vector2 = learning::Vector2f;

class Transform
{

protected:
	Transform();
	Transform(const Transform&) = delete;

public:
	virtual ~Transform() = default;


	Vector2 pos;		// 화면좌표를 그대로 가지는걸로 하자 귀찮으니
	Vector2 scale;		// 유사 콜라이더(크기), 기본 사각형 변의 길이로 치고 AABB해주자


	virtual bool IsIntersectPoint(int x, int y);

};