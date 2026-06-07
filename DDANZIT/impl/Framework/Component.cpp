#include "Component.h"

#include "GameObject.h"

using namespace std;


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


const string& Component::name() const 
{ 
	return _gameObject->_name; 
}


const Tag& Component::tag() const 
{ 
	return _gameObject->_tag; 
}


Transform* const Component::transform()
{ 
	return _gameObject->_transform; 
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
