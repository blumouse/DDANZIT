#pragma once

#include <string>
#include <vector>
#include <queue>

#include "DefineOption.h"
#include "Scene.h"

class Component;
class Lifecycle;
class MonoBehavior;

namespace learning 
{
	struct Vector2f;
}

using Vector2 = learning::Vector2f;


// TODO_LATER: 오브젝트를 상속받기? 는 나중에 고려
class GameObject
{
public:
	//friend class Scene;
	friend class Component;
	friend class Transform;
	friend class MonoBehavior;

	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend void DDANZIT_Run();
	friend void DDANZIT_Finalize();

#pragma region Constructor

protected:
	GameObject();
	GameObject(Scene* scene);
	GameObject(const GameObject&) = default;		// 헷갈린당 가리고 friend가 맞나..?

public:
	virtual ~GameObject();

#pragma endregion


	// 유사 프로퍼티 (정말)
#pragma region Properties

private:
	long index;		// TODO: 이거 할당 ....?

protected:
	Scene* _scene;
	//Scene& scene() { return _scene; }					// 이걸로 관리하지 않아
public:
	Scene* const scene() { return _scene; }


protected:
	std::string _name;
	std::string& name() { return _name; }				// 원본 수정가능 (set)
public:
	const std::string& name() const { return _name; }	// 원본 읽기전용 (get)


protected:
	Tag _tag;
	Tag& tag() { return _tag; }
public:
	const Tag& tag() const { return _tag; }


protected:
	bool _active;
	//bool& active() { return _active; }
public:
	const bool& active() const { return _active; }


private:
	Transform* _transform;
public:
	Transform* const transform() { return _transform; }


	
	// 내부용
private:
	// 나는 아니고~ 부모가 활/비활이래
	bool parentActive = true;

	// Destroy 마킹 (필요 없을수도)
	bool isKilled = false;

#pragma endregion



#pragma region Methods

public:
	void SetActive(bool newActive);


	// 내부용
private:
	void SetParentActive(bool newActive);

#pragma endregion



#pragma region Component

private:
	std::vector<Component*> pComponentList;
	//Component** ppComponents = nullptr;


public:
	int GetComponentCount() const { return pComponentList.size(); }


	template <std::derived_from<Component> T>
	void GetComponents(std::vector<T*>& componentList) const
	{
		componentList.clear();

		for (Component* comp : pComponentList)
		{
			if (T* targetComponent = dynamic_cast<T*>(comp))
				componentList.push_back(targetComponent);
		}
	}

	template <std::derived_from<Component> T>
	T* GetComponent() const 
	{
		for (Component* comp : pComponentList)
		{
			if (T* targetComponent = dynamic_cast<T*>(comp))
				return targetComponent;
		}

		return nullptr;
	}

	template <std::derived_from<Component> T>
	bool TryGetComponent(T*& component) const
	{
		for (Component* comp : pComponentList)
		{
			if (T* targetComponent = dynamic_cast<T*>(comp))
			{
				component = targetComponent;
				return true;
			}
		}

		return false;
	}


protected:
	// 컴포넌트 등록 함수
	// 오브젝트 생성자에서 호출해야함
	// 스크립트 아닌 기본 컴포넌트도 이걸로 등록
	template <std::derived_from<Component> T>
	T* AddComponent()
	{
		// TODO_LATER: 디버그 메세지
		if (pComponentList.size() == MAX_COMPONENT_NUM)
		{
			return;
		}

		
		T* component = new T(this);

		pComponentList.push_back(component);


		if (Lifecycle* lifecycle = dynamic_cast<Lifecycle*>(component))
		{
			if (lifecycle->activeAwake)
				_scene->awakeExecQueue.push(lifecycle);

			if (lifecycle->activeStart)
				_scene->startExecQueue.push(lifecycle);


			if (lifecycle->activeFixedUpdate)
				_scene->fixedUpdateExecList.push_back(lifecycle);

			if (lifecycle->activeUpdate)
				_scene->updateExecList.push_back(lifecycle);

			if (lifecycle->activeLateUpdate)
				_scene->lateUpdateExecList.push_back(lifecycle);

			// TODO?: 다른 On 함수들은 해당 시점에 검사 및 추가
			// 남은거: OnEnable / OnDisable
		}
	}

#pragma endregion



#pragma region Lifecycle

//public:
//	virtual void Awake();
//	virtual void OnEnable();
//	virtual void Start();
//
//	virtual void Update();
//	virtual void FixedUpdate();
//	virtual void LateUpdate();
//
//	virtual void OnDisable();
//	virtual void OnDestroy();

#pragma endregion



#pragma region StaticMethods

	// TODO_LATER: 이것들 원래는 Object의 메서드임

	static GameObject* Instantiate(GameObject* gameObject);
	static GameObject* Instantiate(GameObject* gameObject, Transform* parent);
	static GameObject* Instantiate(GameObject* gameObject, Vector2 position, float angle);
	static GameObject* Instantiate(GameObject* gameObject, Transform* parent, Vector2 position, float angle);
	static GameObject* Instantiate(GameObject* gameObject, Scene* scene);
	static GameObject* Instantiate(GameObject* gameObject, Scene* scene, Vector2 position, float angle);

	static void Destroy(GameObject* gameObject);

	static GameObject* Find(std::string name);
	static GameObject* FindWithTag(Tag tag);


private:
	// ?
	static void RegisterObj(GameObject* pGameObject);
	static void QuitObj(GameObject* pGameObject);


	// 이런것들은 접근 못하게 하는군...
	//static std::vector<GameObject*> GetObjList();
	//static GameObject* Find(long index);

#pragma endregion

};