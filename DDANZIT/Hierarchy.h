#pragma once

#include <vector>

#include "GameObjectDesciptor.h"

class Scene;
class GameObject;


// TODO_LATER: 직렬화를 만들기...?
// 저 += 가 원래는 디스크립터를 추가하는 동작이어야 맞는거같은데..
// 게임 실행중이면 그때나 게임에도 동적 추가해주는거고 말야 암튼 할거면 함수를 따로 빼야함
class Hierarchy
{
public:
	friend class Scene;
	friend class Transform;

public:
	Hierarchy() = default;		// 임시.. 뭔가 해야했었는데 까먹음

	~Hierarchy() = default;

private:
	std::vector<GameObject*> pRootGameObjectList;
	std::vector<GameObject*> pGameObjectList;

public:
	Hierarchy& operator+=(GameObject* go);
	Hierarchy& operator-=(GameObject* go);

	// 위 오퍼레이터랑 같은 동작
	void RegisterGameObject(GameObject* go);
	void QuitGameObject(GameObject* go);

	// 기깔나는 검색기능 없나 전체 리스트로 보여주지 뭐
	std::vector<GameObject*> GetObjectList();
	GameObject* GetObjectByIndex(int index);



	// 직렬화 / 로드
private:
	//std::vector<GameObjectDesciptor> gameObjectDescList;

	//void LoadGameObjects();
	//void UnloadGameObjects();
};

