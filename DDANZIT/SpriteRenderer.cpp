#include "SpriteRenderer.h"

#include "GameObject.h"


#pragma region Constructor

SpriteRenderer::SpriteRenderer(GameObject* pGameObject) : 
	Component(pGameObject), sprite(SpriteIndex::None), color(Color(1.0f, 1.0f, 1.0f, 1.0f)), flipX(false), flipY(false)
{

}

#pragma endregion



#pragma region Clone

Component* SpriteRenderer::Clone() const
{
	return new SpriteRenderer(*this);
}

#pragma endregion



#pragma region Properties

#pragma endregion



#pragma region Methods

#pragma endregion

