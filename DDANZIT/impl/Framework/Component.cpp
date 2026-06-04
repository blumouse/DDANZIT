#include "Component.h"

#include "GameObject.h"


#pragma region Constructor

Component::Component(GameObject* pGameObject) : _gameObject(pGameObject), _active(true), parentActive(pGameObject->active())
{
	
}

Component::Component(GameObject* pGameObject, bool active) : _gameObject(pGameObject), _active(active), parentActive(pGameObject->active())
{

}

#pragma endregion



#pragma region Properties

GameObject* const Component::gameObject()
{
	if (isKilled)	// 이게 맞는지도
	{
		// TODO_LATER: 디버그 메세지
		return nullptr;
	}

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
	if (parentActive == newActive)
		return;

	parentActive = newActive;
}

#pragma endregion
