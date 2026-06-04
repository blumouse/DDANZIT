#include "Transform.h"

#include "DefineOption.h"

#include "GameObject.h"

using namespace std;


#pragma region Constructor

Transform::Transform(GameObject* pGameObject) : 
	Component(pGameObject), _parent(HIERARCY_ROOT), _position(Vector2(0, 0)), _direction(Vector2(1.0f, 0)), _scale(Vector2(1.0f, 1.0f)), _depth(0)
{
	//RegisterTransform(this);
}

Transform::Transform(const Transform& other) :
	Component(other)
{
	_position = other._position;
	_direction = other._direction;
	_scale = other._scale;
	_depth = other._depth;
	_parent = HIERARCY_ROOT;

	// pChildList는 안건드림 자식 빈상태 그대로
}

#pragma endregion



#pragma region Properties

void Transform::SetAngle(float degree)
{ 
	float radians = degree * DEG2RAD;

	_direction.x = cos(radians);
	_direction.y = sin(radians);
}

float Transform::angle() const 
{ 
	float radians = atan2(_direction.y, _direction.x);

	return radians * RAD2DEG;
}

void Transform::SetDepth(int depth) 
{
	if (depth < 0 || depth >= MAX_LAYER_NUM)
	{
		// DEBUG: 레이어 범위 밖
		return;
	}

	_depth = depth;
}

#pragma endregion



#pragma region Clone

Transform* Transform::Clone() const
{
	return new Transform(*this);
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