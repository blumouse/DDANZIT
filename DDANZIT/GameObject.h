#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <queue>

#include "DefineOption.h"

#include "DDANZIT_Core.h"
#include "Scene.h"
#include "Component.h"
#include "MonoBehavior.h"

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
	GameObject(const GameObject&);		// TODO: 인스턴트를 위한 재정의 근데 이것도 상위에서 처리될라나?

public:
	virtual ~GameObject();

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
	bool parentActive;

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
			if (comp->isKilled)
				continue;

			if (T* targetComponent = dynamic_cast<T*>(comp))
				componentList.push_back(targetComponent);
		}
	}

	template <std::derived_from<Component> T>
	T* GetComponent() const 
	{
		for (Component* comp : pComponentList)
		{
			if (comp->isKilled)
				continue;

			if (T* targetComponent = dynamic_cast<T*>(comp))
				return targetComponent;
		}

		// TODO_LATER: 그런 컴포넌트 없음 메세지
		return nullptr;
	}

	template <std::derived_from<Component> T>
	bool TryGetComponent(T*& component) const
	{
		for (Component* comp : pComponentList)
		{
			if (comp->isKilled)
				continue;

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

		// TODO: 생각해보니까 이것도... 인스턴트가 프레임 내에서 일어나니까..? 아닌가 시작은 이미됐으니
		if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(component))
		{
			if (b->activeAwake)
				DDANZIT_Core::awakeExecQueue.push(b);

			if (b->activeOnEnable && b->isActiveAndEnabled())
				DDANZIT_Core::onEnableExecQueue.push(b);

			if (b->activeStart)
				DDANZIT_Core::startExecQueue.push(b);


			// 활성화 여부에 따라.. 안넣을수도있음
			if (b->isActiveAndEnabled())
				DDANZIT_Core::RegisterUpdateExecLists(b);
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


	static void DontDestroyOnLoad(GameObject* gameObject);


private:
	// ?
	static void RegisterObj(GameObject* pGameObject);
	static void QuitObj(GameObject* pGameObject);


	// 이런것들은 접근 못하게 하는군...
	//static std::vector<GameObject*> GetObjList();
	//static GameObject* Find(long index);

#pragma endregion

};