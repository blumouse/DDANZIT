#include "aria.h"

#include "DDANZITEngine.h"
#include "characterAnimation.h"


aria::aria(Scene* scene) : Draw2D(this)
{
	//사용할 컴포넌트 추가
	AddComponent<characterAnimation>();
}

GameObject* aria::Clone() const
{
	return new aria(*this);
}