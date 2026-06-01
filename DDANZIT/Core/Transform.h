#pragma once

#include <vector>

#include "DefineOption.h"
#include "Utillity.h"
#include "Component.h"


// 고정 컴포넌트(유사)로 바꿔야한다

using Vector2 = learning::Vector2f;

class GameObject;

class Transform : public Component
{
public:
	friend class GameObject;


#pragma region Constructor

private:
	Transform() = delete;
	Transform(const Transform&) = default;	// 이거 괜찮나?
	Transform(GameObject* gameObject);

public:
	virtual ~Transform() = default;

#pragma endregion



#pragma region Properties

	// 이것도 매크로로 받아와서 2d / 3d 구분해주기?
#ifdef USE_2D
	
private:
	Vector2 _position;
public:
	Vector2& position() { return _position; }				// 원본 수정가능 (set)
	const Vector2& position() const { return _position; }	// 원본 읽기전용 (get)


private:
	Vector2 _scale;
public:
	Vector2& scale() { return _scale; }
	const Vector2& scale() const { return _scale; }

	// TODO: rotation <-> angle 상호 변환해야함 Set 함수를 써야겠네
private:
	Vector2 _rotation;
public:
	Vector2& rotation() { return _rotation; }
	const Vector2& rotation() const { return _rotation; }


private:
	float _angle;
public:
	float& angle() { return _angle; }
	const float& angle() const { return _angle; }


private:
	unsigned int _depth;
public:
	// 인덱스 방식이기 때문에 음수는 없을 예정
	unsigned int& depth() { return _depth; }
	const int& depth() const { return _depth; }


#endif // USE_2D


#pragma endregion



#pragma region Tree

private:
	Transform* _parent;
public:
	// nullptr이 Root
	Transform* parent() const { return _parent; }


private:
	std::vector<Transform*> pChildList;
public:
	const int& childCount() const { return pChildList.size(); }


	// set됐을때 자동으로 연결할 방법이.. 지금 구조론 없는걸 인자로 받기
	void SetParent(Transform* parent);
	Transform* GetChild(int index) const;
	//void DetachChildren();

	// 내부 메서드
private:
	void AddChild(Transform* child);
	void RemoveChild(Transform* child);

#pragma endregion



#pragma region Methods


#pragma endregion


};