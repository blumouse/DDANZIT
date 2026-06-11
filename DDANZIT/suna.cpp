#include "suna.h"

#include "DDANZITEngine.h"


suna::suna(Scene* scene) : Draw2D(this)
{
	//사용할 컴포넌트 추가
	AddComponent<Script>();
}

GameObject* suna::Clone() const
{
	return new suna(*this);
}