#include "Component.h"


#pragma region Constructor

Component::Component(GameObject* pGameObject) : _gameObject(pGameObject), _active(true)
{
	
}

#pragma endregion



#pragma region Properties

GameObject* const Component::gameObject()
{
	return _gameObject;
}

#pragma endregion



#pragma region Methods

void Component::SetActive(bool newActive)
{
	if (_active == newActive)
		return;

	_active = newActive;
}

void Component::SetParentActive(bool newActive)
{
	if (_active == newActive)
		return;

	_active = newActive;
}

#pragma endregion
