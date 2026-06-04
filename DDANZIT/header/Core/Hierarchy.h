#pragma once

#include <concepts>
#include <string>
#include <vector>

#include "DefineOption.h"
#include "GameObjectDesciptor.h"

#include "GameObject.h"
#include "Transform.h"

class Scene;
class GameObject;
class Component;


// TODO_LATER: 직렬화를 만들기...?
// 필요한 동작이 (적어도) 세개네? 생성해서 넣어주기 / 그냥 등록하기 / 설명서만 넣기
class Hierarchy
{
public:
	friend class Scene;
	friend class Transform;

#pragma region Constructor

public:
	Hierarchy() = delete;		// 임시.. 뭔가 해야했었는데 까먹음
	Hierarchy(Scene* scene);

	~Hierarchy() = default;

#pragma endregion



#pragma region Properties

private:
	std::vector<GameObject*> pRootGameObjectList;
	std::vector<GameObject*> pGameObjectList;

	Scene* scene;

#pragma endregion



#pragma region Methods

public:
	// 이쪽은 순수하게 가입 / 탈퇴만
	Hierarchy& operator+=(GameObject* go);
	Hierarchy& operator-=(GameObject* go);

	// 위 오퍼레이터랑 같은 동작.. 이면 안된다
	// 이쪽은 생성까지 해주자! 그럼 제네릭으로도 받아야함?
	GameObject* RegisterGameObject();

	template <std::derived_from<GameObject> T>
	T* RegisterGameObject()
	{
		if (pGameObjectList.size() == MAX_SCENE_GAME_OBJECT_NUM)
		{
			// DEBUG: 디버그 메세지
			return;
		}

		T* go = new T(scene);		// 여기서 유저설정한 컴포넌트들 일단 붙어서 나옴


		pGameObjectList.push_back(go);
		go->ownerHierarchy = this;

		if (go->transform()->parent() == HIERARCY_ROOT)
			pRootGameObjectList.push_back(go);

		for (Component* comp : go->pComponentList)
		{
			if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(comp))
			{
				go->InitializeLifecycle(b);
			}
		}

		go->isInitialized = true;

		return go;
	}


	// 기깔나는 검색기능 없나 전체 리스트로 보여주지 뭐
	std::vector<GameObject*> GetObjectList();

	GameObject* GetObjectByTag(Tag tag);
	GameObject* GetObjectByName(std::string name);

#pragma endregion



#pragma region Serialize

	// 직렬화 / 로드
private:
	//std::vector<GameObjectDesciptor> gameObjectDescList;

	//void LoadGameObjects();
	//void UnloadGameObjects();

#pragma endregion

};

