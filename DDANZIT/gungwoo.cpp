#include "gungwoo.h"

#include "DDANZITEngine.h"

#include "characterAnimation.h"

gungwoo::gungwoo(Scene* scene) : Draw2D(this)
{
	//사용할 컴포넌트 추가
	AddComponent<characterAnimation>();
}

GameObject* gungwoo::Clone() const
{
	return new gungwoo(*this);
}