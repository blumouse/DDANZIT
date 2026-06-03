#pragma once

#include <vector>

#include "DefineOption.h"
#include "Component.h"

#include "Utillity.h"


// 고정 컴포넌트(유사)로 바꿔야한다

using Vector2 = learning::Vector2f;

class GameObject;

class Transform : public Component
{
public:
	friend class DDANZIT_Core;
	friend class GameObject;

	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend void DDANZIT_Run();
	friend void DDANZIT_Finalize();

#pragma region Constructor

private:
	Transform() = delete;
	Transform(const Transform&) = default;	// 이거 괜찮나?
	Transform(GameObject* gameObject);
	Transform(GameObject* pGameObject, bool active) = delete;

public:
	virtual ~Transform() = default;

#pragma endregion



#pragma region Properties

	// TODO: 부모 오브젝트 기준으로 움직이게 하기
#ifdef PROPS_MODE_2D
	
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
	// 아니면.. 오퍼레이터 정의해도됨 헉!
private:
	Vector2 _rotation;
public:
	Vector2& rotation() { return _rotation; }
	const Vector2& rotation() const { return _rotation; }


private:
	float _angle;
public:
	// 보기좋은 degree
	float& angle() { return _angle; }
	const float& angle() const { return _angle; }


private:
	unsigned int _depth;
public:
	// 인덱스 방식이기 때문에 음수는 없을 예정
	unsigned int& depth() { return _depth; }
	const int& depth() const { return _depth; }

#endif // PROPS_MODE_2D


#pragma endregion



#pragma region HierarchyTree

private:
	Transform* _parent;
public:
	// nullptr이면 root, root는 리스트로 씬에서 갖기
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

public:
	void SetActive(bool newActive) override;


	// 내부용
private:
	void SetParentActive(bool newActive) override;

#pragma endregion


};