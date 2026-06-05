#include "Draw2D.h"

#include "GameObject.h"
#include "Component.h"
#include "SpriteRenderer.h"


#ifdef PROPS_MODE_2D


#pragma region Constructor

Draw2D::Draw2D(GameObject* gameObject) : gameObject(gameObject)
{

}

#pragma endregion



#pragma region IDrawable

void Draw2D::Draw(HDC hdc) 
{
	// TODO: 이거 직접 찾지말고 (유니티도 그러니) 칸 하나 딱 뚫어놓은 다음에
	// 컴포넌트가 여기에 와서 넣어주고 가자 Add 할때
	// 이렇게 생각하니까 기본 컴포넌트는 고유의 초기화 동작이 좀 있어야되네?
	// 암튼 스프라이트는 AddComponent 시점에 자신을 넣어주고 가야함

	for (Component* comp : gameObject->pComponentList)
	{
		if (!comp->isActiveAndEnabled())
			continue;
		
		if (SpriteRenderer* sp = dynamic_cast<SpriteRenderer*>(comp))
		{
			// TODO: 속성가지고 와랄라라이
			// 여기서 hdc를 쓰지말까..? 커맨드로 갖다가 그러니까 스케치를 하는거야
			// 생각해보면 또 하나.. 카메라를 도입한다면 결국 그리는 시점은 카메라가 보는 시점이네 오호..
		}
	}
}


int Draw2D::GetLayer() 
{
	return layer;
}

void Draw2D::SetLayer(int layer) 
{
	this->layer = layer;
}

#pragma endregion


#endif // PROPS_MODE_2D