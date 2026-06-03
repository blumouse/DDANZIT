#include "Transform.h"

#include "DefineOption.h"

#include "GameObject.h"

using namespace std;


#pragma region Constructor

Transform::Transform(GameObject* pGameObject) : 
	Component(pGameObject), _parent(nullptr), _position(Vector2(0, 0)), _rotation(Vector2(1, 0)), _angle(0), _scale(Vector2(1, 1)), _depth(0)
{
	//RegisterTransform(this);
}

#pragma endregion



#pragma region Methods

void Transform::SetActive(bool newActive)
{
	// 경고

}


void Transform::SetParentActive(bool newActive)
{
	// 딱히 의미없음
}

#pragma endregion



#pragma region HierarchyTree

void Transform::SetParent(Transform* parent)
{
	if (_parent == parent)
		return;

	if (parent == HIERARCY_ROOT)
	{
		_gameObject->_scene->hierarchy.pRootGameObjectList.push_back(_gameObject);

		_parent = parent;
		return;
	}
	// (parent != HIERARCY_ROOT)

	if (_gameObject->_scene != parent->_gameObject->_scene)
	{
		// DEBUG: 다른 씬의 오브젝트임
		return;
	}

	if (_parent != HIERARCY_ROOT)
		_parent->RemoveChild(this);

	parent->AddChild(this);
	

	_parent = parent;
}

Transform* Transform::GetChild(int index) const
{
	if (index < 0 || index > pChildList.size())
		return nullptr;

	return pChildList[index];
}


void Transform::AddChild(Transform* child)
{
	pChildList.push_back(child);
}

void Transform::RemoveChild(Transform* child)
{
	pChildList.erase(remove(pChildList.begin(), pChildList.end(), child), pChildList.end());
}

#pragma endregion