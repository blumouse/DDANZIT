#pragma once

#include <concepts>
#include <string>
#include <vector>

#include "DefineOption.h"
#include "GameObjectDesciptor.h"

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


	// 그냥 여기서만 생성 (빈 오브젝트)
	GameObject* AddGameObject();

	// 사용자 정의 컴포넌트 미리 달린 오브젝트
	template <std::derived_from<GameObject> T>
	T* AddGameObject();


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