#include "GameObject.h"

#include "DefineOption.h"

#include "DDANZIT.h"
#include "DDANZIT_Core.h"

#include "SceneManager.h"
#include "Scene.h"
#include "Component.h"
#include "MonoBehavior.h"

#include "Transform.h"

// TODO: MAX_...어쩌고를 인덱스 넘으려고 하면 방어를 해주고싶은데
// 그냥 팩토리면 방어되나? new를 가리고 friend로 빼주고서..
// ...강제는 못하겠고 차피 뭐 게임에서 돌아갈라면 등록을 해야되니까
// 컴포넌트도 맡기는 함수가 있어야겠다 암튼 (결국)
// 하이라키에 등록하거나 Instantiate하거나 하면 그후에 내부에서 등록.. 할때!
// 아닌가 굳이 방어할거 없나..? 이건 너무 예외처리 영역인데 일단 킵

using namespace std;


#pragma region Constructor

GameObject::GameObject() : 
	_active(true), parentActive(true), _tag(Tag::Default)
{
	if (SceneManager::mainScene)
		_scene = SceneManager::mainScene;
	else 
	{
		// 굉장한 오류
		return;
	}

	_transform = new Transform(this);		// ?? 이건 괜찮나? 뭐 고정이긴 한데..
}

GameObject::GameObject(Scene* scene) : 
	_active(true), parentActive(true), _tag(Tag::Default)
{
	if (scene)
		_scene = scene;
	else
	{
		// 오류
		return;
	}

	_transform = new Transform(this);
}

GameObject::GameObject(Scene* scene, bool parentActive) : 
	_active(true), parentActive(parentActive), _tag(Tag::Default)
{

}

GameObject::~GameObject()
{
	// TODO_LATER: 진짜 삭제보다 모종의 재활용?

	for (Component* comp : pComponentList)
		delete comp;

	delete _transform;
}

#pragma endregion



#pragma region Properties

Transform* const GameObject::transform()
{
	if (isKilled)
	{
		// TODO_LATER: 디버그 메세지
		return nullptr;
	}

	return _transform;
}

#pragma endregion



#pragma region Methods

void GameObject::SetActive(bool newActive) 
{
	if (_active == newActive)
		return;


	for (Transform* tr : _transform->pChildList)
		tr->_gameObject->SetParentActive(newActive);

	for (Component* comp : pComponentList)
		comp->SetParentActive(newActive);

	// 모노비헤이비어 단에서 큐 넣어줌;

	_active = newActive;
}

void GameObject::SetParentActive(bool newActive)
{
	if (parentActive == newActive)
		return;


	for (Transform* tr : _transform->pChildList)
		tr->_gameObject->SetParentActive(newActive);

	for (Component* comp : pComponentList)
		comp->SetParentActive(newActive);

	parentActive = newActive;
}


void GameObject::SetSceneAndDetach(Scene* scene) 
{
	if (!scene->isLoaded)
		return;

	_scene = scene;

	for (Transform* tr : _transform->pChildList)
	{
		tr->SetParent(HIERARCY_ROOT);
	}
}

void GameObject::SetSceneRecursive(Scene* scene)
{
	if (!scene->isLoaded)
		return;

	_scene = scene;

	for (Transform* tr : _transform->pChildList)
	{
		tr->_gameObject->SetSceneRecursive(scene);
	}
}

#pragma endregion



#pragma region Component

#pragma endregion



#pragma region StaticMethods

GameObject* GameObject::Instantiate(GameObject* gameObject)
{
	if (gameObject == nullptr)
	{
		return nullptr;
	}

	if (gameObject->isKilled)
	{
		return nullptr;
	}

	// 메인 씬 루트에 추가
}

GameObject* GameObject::Instantiate(GameObject* gameObject, Transform* parent)
{
	if (gameObject == nullptr)
	{
		return nullptr;
	}

	if (gameObject->isKilled)
	{
		return nullptr;
	}

	// 부모 씬 부모 자식으로 추가
}

GameObject* GameObject::Instantiate(GameObject* gameObject, Vector2 position, float angle)
{
	if (gameObject == nullptr)
	{
		return nullptr;
	}

	if (gameObject->isKilled)
	{
		return nullptr;
	}

}

GameObject* GameObject::Instantiate(GameObject* gameObject, Transform* parent, Vector2 position, float angle)
{
	if (gameObject == nullptr)
	{
		return nullptr;
	}

	if (gameObject->isKilled)
	{
		return nullptr;
	}

}

GameObject* GameObject::Instantiate(GameObject* gameObject, Scene* scene)
{
	if (gameObject == nullptr)
	{
		return nullptr;
	}

	if (gameObject->isKilled)
	{
		return nullptr;
	}

	// 해당 씬 루트에 추가
}

GameObject* GameObject::Instantiate(GameObject* gameObject, Scene* scene, Vector2 position, float angle)
{
	if (gameObject == nullptr)
	{
		return nullptr;
	}

	if (gameObject->isKilled)
	{
		return nullptr;
	}


}


// 실제 파괴는 프레임 가장 마지막에 일어난다
void GameObject::Destroy(GameObject* gameObject) 
{
	if (gameObject == nullptr)
	{
		return;
	}

	if (gameObject->isKilled)
	{
		return;
	}

	gameObject->isKilled = true;

	for (Component* comp : gameObject->pComponentList)
	{
		comp->isKilled = true;

		if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(comp))
		{
			DDANZIT_Core::QuitUpdateExecLists(b);		// 직접 호출


			if (b->activeOnDisable)
				DDANZIT_Core::onDisableExecQueue.push(b);

			if (b->activeOnDestroy)
				DDANZIT_Core::onDestroyExecQueue.push(b);
		}
	}

	for (Transform* tr : gameObject->_transform->pChildList)
	{
		Destroy(tr->_gameObject);
	}
}

GameObject* GameObject::Find(string name) 
{

}

GameObject* GameObject::FindWithTag(Tag tag) 
{

}

#pragma endregion