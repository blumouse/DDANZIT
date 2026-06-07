#pragma once

#include <vector>

#include "DefineOption.h"
#include "Component.h"

#include "Utillity.h"

using Vector2 = learning::Vector2f;

class GameObject;


class Transform : public Component
{
public:
	friend class DDANZIT_Core;
	friend class GameObject;
	friend class Camera;

	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend void DDANZIT_Run();
	friend void DDANZIT_Finalize();

#pragma region Constructor

private:
	Transform() = delete;
	Transform(const Transform& other);		// 직접 구현해야겠다; 자식까지 전부 복제는 그냥 내가 안할래
	Transform(GameObject* gameObject);
	Transform(GameObject* pGameObject, bool active) = delete;

public:
	~Transform() = default;

#pragma endregion



#pragma region Clone

private:
	Component* Clone() const;
	Transform* CloneTransform() const;

#pragma endregion



#pragma region Properties

	// TODO: 이거 따로 빼기 라이브러리같은거?

	static constexpr float DEG2RAD = 3.14159265f / 180.0f;
	static constexpr float RAD2DEG = 180.0f / 3.14159265f;

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


private:
	Vector2 _direction;
public:
	// 유니티엔 없는 방향벡터
	Vector2& direction() { return _direction; }
	const Vector2& direction() const { return _direction; }


public:
	// 보기좋은 degree
	void SetAngle(float degree);
	float angle() const;


private:
	unsigned int _depth;
public:
	// 인덱스 방식이기 때문에 음수는 없을 예정 작을수록 위쪽
	void SetDepth(int depth);
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

	// TODO: 로컬 / 월드(+부모) 기준 좌표 반환용 함수 만들기
	// 유니티 그럼 기본 포지션 찍으면 나오는게 어느 기준인거지?


	// 내부용
private:
	void SetParentActive(bool newActive) override;

#pragma endregion


};