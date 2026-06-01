#include "Component.h"


#pragma region Constructor

Component::Component(GameObject* pGameObject) : _gameObject(pGameObject) 
{
	
}

#pragma endregion



#pragma region Properties

GameObject* const Component::gameObject()
{
	return _gameObject;
}

#pragma endregion



#pragma region LifeCycles


#pragma endregion
