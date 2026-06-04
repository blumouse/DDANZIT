#include "Draw2D.h"

#include "GameObject.h"
#include "Component.h"
#include "SpriteRenderer.h"


#pragma region Constructor

Draw2D::Draw2D(GameObject* gameObject) : gameObject(gameObject)
{

}

#pragma endregion


#pragma region IDrawable

void Draw2D::Draw(HDC hdc) 
{
	for (Component* comp : gameObject->pComponentList)
	{
		if (!comp->isActiveAndEnabled())
			continue;
		
		if (SpriteRenderer* sp = dynamic_cast<SpriteRenderer*>(comp))
		{
			// TODO: 속성가지고 와랄라라이
			// 여기서 hdc를 쓰지말까..?
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
