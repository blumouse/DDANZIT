#include "MonoBehavior.h"

#include "GameObject.h"
#include "Scene.h"


#pragma region Constructor

MonoBehavior::MonoBehavior(GameObject* pGameObject) : Component(pGameObject)
{

}

#pragma endregion



#pragma region Methods

void MonoBehavior::SetActive(bool newActive)
{
	if (_active == newActive)
		return;

	if (newActive /*== true*/)
	{
		// OnEnable 리스트에 추가
		_gameObject->_scene->onEnableExecQueue.push(this);
	}
	else
	{
		// 유사
		_gameObject->_scene->onDisableExecQueue.push(this);
	}

	_active = newActive;
}

void MonoBehavior::SetParentActive(bool newActive)
{
	if (_active == newActive)
		return;

	if (newActive /*== true*/)
	{
		_gameObject->_scene->onEnableExecQueue.push(this);
	}
	else
	{
		// 유사
		_gameObject->_scene->onDisableExecQueue.push(this);
	}

	_active = newActive;
}

#pragma endregion