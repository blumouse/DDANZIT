#pragma once

#include <string>
#include <vector>

#include "DefineOption.h"

class Component;


// TODO_LATER: 오브젝트를 상속받기? 는 나중에 고려
class GameObject
{
public:
	friend class Transform;


#pragma region Constructor

protected:
	GameObject();
	GameObject(const GameObject&) = default;		// 헷갈린당 가리고 friend가 맞나..?

public:
	virtual ~GameObject();

#pragma endregion


	// 유사 프로퍼티 (정말)
#pragma region Properties

private:
	long index;


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
	bool& active() { return _active; }
public:
	const bool& active() const { return _active; }


private:
	Transform* _transform;
public:
	Transform* const transform();


#pragma endregion


	// 이것들은 잠시 치워두기로...
	//ObjectData* data;
	//ObjectVisual* visual;
	//ObjectLogic* logic;

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



	// TODO_LATER: 이건 좀 후순위로.. 유니티에 동일한 동작이 뭐가있지? 없나..?
	//template <std::derived_from<Component> T>
	//void RegisterComponent(T* component) {
	//	components.push_back(component);
	//}

#pragma endregion



#pragma region Methods

public:
	void SetActive(bool newActive);



#pragma endregion



#pragma region Lifecycle

	virtual void Awake();
	virtual void OnEnable();
	virtual void Start();
	virtual void Update();
	virtual void FixedUpdate();

	virtual void OnDisable();
	virtual void OnDestroy();

#pragma endregion



	// 이방식이 생각보다 맞았다; (유니티 기준) 클래스 정적메서드
#pragma region StaticMethods

	// TODO_LATER: 이것들 원래는 Object의 메서드임

	static GameObject* Instantiate(GameObject* gameObject);
	static GameObject* Instantiate(GameObject* gameObject, Transform* parent);
	static GameObject* Instantiate(GameObject* gameObject, Vector2 position, float angle);
	//static GameObject* Instantiate(GameObject* gameObject, Scene scene);

	static void Destroy(GameObject*& gameObject);

	static GameObject* Find(std::string name);
	static GameObject* FindWithTag(Tag tag);


private:
	// ?
	static void RegisterObj(GameObject* pGameObject);
	static void QuitObj(GameObject* pGameObject);


	// 이런것들은 접근 못하게 하는군...
	//static std::vector<GameObject*> GetObjList();
	//static GameObject* Find(long index);
	

	//static GameObject* GetObject(ObjectData* data);
	//static GameObject* GetObject(ObjectVisual* visual);
	//static GameObject* GetObject(ObjectLogic* logic);

#pragma endregion


};