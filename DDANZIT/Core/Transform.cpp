#include "Transform.h"
#include "GameObject.h"

using namespace std;


#pragma region Constructor

Transform::Transform(GameObject* pGameObject) : 
	Component(pGameObject), _parent(nullptr), _position(Vector2(0, 0)), _rotation(Vector2(1, 0)), _angle(0), _scale(Vector2(1, 1)), _depth(0)
{
	//RegisterTransform(this);
}

#pragma endregion



#pragma region Tree

void Transform::SetParent(Transform* parent)
{
	if (_parent == parent)
		return;

	if (_parent != nullptr)
		_parent->RemoveChild(this);

	if (parent != nullptr)
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
	pChildList.erase(std::remove(pChildList.begin(), pChildList.end(), child), pChildList.end());
}

#pragma endregion