#include "Transform.h"

#include "DefineOption.h"

#include "Scene.h"

#include "GameObject.h"

#ifdef PROPS_MODE_2D
#include "Draw2D.h"

#endif // PROPS_MODE_2D

using namespace std;


#pragma region Constructor

Transform::Transform(GameObject* pGameObject) : 
	Component(pGameObject), _parent(HIERARCY_ROOT), _localPosition(Vector2(0, 0)), _localDirection(Vector2(1.0f, 0)), _localScale(Vector2(1.0f, 1.0f)), _depth(0)
{
	//RegisterTransform(this);
}

Transform::Transform(const Transform& other) :
	Component(other)
{
	_localPosition = other._localPosition;
	_localDirection = other._localDirection;
	_localScale = other._localScale;
	_depth = other._depth;
	_parent = HIERARCY_ROOT;

	// pChildList는 안건드림 자식 빈상태 그대로
}

#pragma endregion



#pragma region Properties

// TODO_LATER: 이것들 캐싱해서 최적화하기?
#ifdef PROPS_MODE_2D

void Transform::SetPosition(Vector2 newPosition)
{
	Vector2 parentPosition = Vector2(0,0);
	Transform* par = _parent;

	while (par != HIERARCY_ROOT)
	{
		parentPosition += par->_parent->_localPosition;
		par = par->_parent;
	}

	_localPosition = newPosition - parentPosition;
}

Vector2 Transform::position() const 
{ 
	Vector2 worldPosition = _localPosition;
	Transform* par = _parent;

	while (par != HIERARCY_ROOT)
	{
		worldPosition += par->_parent->_localPosition;
		par = par->_parent;
	}

	return worldPosition;
}


void Transform::SetScale(Vector2 newScale)
{
	Vector2 parentScale = Vector2(0, 0);
	Transform* par = _parent;

	while (par != HIERARCY_ROOT)
	{
		parentScale += par->_parent->_localScale;
		par = par->_parent;
	}

	_localScale = newScale - parentScale;
}

Vector2 Transform::scale() const
{
	Vector2 worldScale = _localScale;
	Transform* par = _parent;

	while (par != HIERARCY_ROOT)
	{
		worldScale += par->_parent->_localScale;
		par = par->_parent;
	}

	return worldScale;
}


void Transform::SetDirection(Vector2 newDirection)
{
	Vector2 parentDirection = Vector2(0, 0);
	Transform* par = _parent;

	while (par != HIERARCY_ROOT)
	{
		parentDirection += par->_parent->_localDirection;
		par = par->_parent;
	}

	_localDirection = (newDirection - parentDirection).Normalized();
}

Vector2 Transform::direction() const
{
	Vector2 worldDirection = _localDirection;
	Transform* par = _parent;

	while (par != HIERARCY_ROOT)
	{
		worldDirection += par->_parent->_localDirection;
		par = par->_parent;
	}

	return worldDirection.Normalized();
}


void Transform::SetAngle(float degree)
{ 
	float radians = degree * DEG2RAD;

	_localDirection.x = cos(radians);
	_localDirection.y = sin(radians);
}

float Transform::angle() const 
{ 
	float radians = atan2(_localDirection.y, _localDirection.x);

	return radians * RAD2DEG;
}


void Transform::SetDepth(int depth) 
{
	if (depth < 0 || depth >= MAX_LAYER_NUM)
	{
		// DEBUG: 레이어 범위 밖
		return;
	}

	if (Draw2D* draw = dynamic_cast<Draw2D*>(_gameObject))
	{
		draw->SetLayer(depth);
	}

	_depth = depth;
}

#endif // PROPS_MODE_2D

#pragma endregion



#pragma region Clone

Component* Transform::Clone() const
{
	// ASSERT: 사용되지 않음
	return nullptr;
}

Transform* Transform::CloneTransform() const
{
	// ASSERT: 사용되지 않음
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