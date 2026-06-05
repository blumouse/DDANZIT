#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <queue>

#include "DefineOption.h"

class Lifecycle;

namespace learning 
{
	struct Vector2f;
}

using Vector2 = learning::Vector2f;


// TODO_LATER: 오브젝트를 상속받기? 는 나중에 고려
class GameObject
{
public:
	friend class DDANZIT_Core;
	friend class SceneManager;

	friend class Scene;
	friend class Hierarchy;

	friend class Draw2D;

	friend class Component;
	friend class Transform;
	friend class MonoBehavior;

	//friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	//friend void DDANZIT_Run();
	//friend void DDANZIT_Finalize();

#pragma region Constructor

protected:
	GameObject();
	GameObject(Scene* scene);
	GameObject(Scene* scene, bool parentActive);
	GameObject(const GameObject& other);

public:
	virtual ~GameObject();

#pragma endregion



#pragma region Clone

private:
	virtual GameObject* Clone() const;

#pragma endregion


	// 유사 프로퍼티 (정말)
#pragma region Properties

protected:
	Scene* _scene;
	//Scene& scene() { return _scene; }					// 이걸로 관리하지 않아
public:
	Scene* const scene() { return _scene; }


protected:
	std::string _name;
public:
	std::string& name() { return _name; }				// 원본 수정가능 (set)
	const std::string& name() const { return _name; }	// 원본 읽기전용 (get)


protected:
	Tag _tag;
public:
	Tag& tag() { return _tag; }
	const Tag& tag() const { return _tag; }


protected:
	bool _active;
	//bool& active() { return _active; }
public:
	const bool& active() const { return _active; }


private:
	Transform* _transform;
public:
	Transform* const transform();


	
	// 내부용
private:
	// 나는 아니고~ 부모가 활/비활이래
	bool parentActive;

	// pGameObjectList에 들어있는가
	Hierarchy* ownerHierarchy = nullptr;

	// 생성 / 및 생성 시 라이프사이클 호출 끝났음 플래그
	bool isInitialized = false;

	// Destroy 마킹
	// 이건 참조불가로 만드는 목적, 실제 파괴는 프레임 마지막에 일어나니
	bool isKilled = false;

#pragma endregion



#pragma region Methods

public:
	void SetActive(bool newActive);


	// 내부용
private:
	void SetParentActive(bool newActive);

	void SetSceneAndDetach(Scene* scene);
	void SetSceneRecursive(Scene* scene);

	// 런타임 여부에 따라 동작이 다르다
	void InitializeLifecycle(MonoBehavior* behavior);

#pragma endregion



#pragma region Component

private:
	std::vector<Component*> pComponentList;
	//Component** ppComponents = nullptr;


public:
	int GetComponentCount() const { return pComponentList.size(); }


	template <std::derived_from<Component> T>
	void GetComponents(std::vector<T*>& componentList) const;

	template <std::derived_from<Component> T>
	T* GetComponent() const;

	template <std::derived_from<Component> T>
	bool TryGetComponent(T*& component) const;


public:
	// 컴포넌트 등록 함수 (MonoBehavior 스크립트 포함)
	// 생성자에서 등록하세요
	template <std::derived_from<Component> T>
	T* AddComponent();

#pragma endregion



#pragma region StaticMethods

	// TODO_LATER: 이것들 원래는 Object의 메서드임
public:
	static GameObject* Instantiate(GameObject* gameObject);
	static GameObject* Instantiate(GameObject* gameObject, Transform* parent);
	static GameObject* Instantiate(GameObject* gameObject, Vector2 position, Vector2 direction);
	static GameObject* Instantiate(GameObject* gameObject, Vector2 position, Vector2 direction, Transform* parent);
	static GameObject* Instantiate(GameObject* gameObject, Scene* scene);

	static void Destroy(GameObject* gameObject);

	static GameObject* Find(std::string name);
	static GameObject* FindWithTag(Tag tag);


	static void DontDestroyOnLoad(GameObject* gameObject);

#pragma endregion

};