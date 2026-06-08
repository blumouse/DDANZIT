#include "GameObject.h"

#include "DefineOption.h"

#include "DDANZIT.h"
#include "DDANZIT_Core.h"
#include "Application.h"

#include "SceneManager.h"
#include "Scene.h"

#include "Component.h"
#include "MonoBehavior.h"

#include "Transform.h"
#include "SpriteRenderer.h"
#include "Camera.h"

#ifdef PROPS_MODE_2D
#include "Draw2D.h"

#endif // PROPS_MODE_2D

using namespace std;


#pragma region Constructor

GameObject::GameObject() : 
	_active(true), parentActive(true), _name("GameObject"), _tag(Tag::Default)
{
	if (SceneManager::mainScene)
		_scene = SceneManager::mainScene;
	else 
	{
		// DEBUG: 굉장한 오류
		return;
	}

	_transform = new Transform(this);		// ?? 이건 괜찮나? 뭐 고정이긴 한데..
}

GameObject::GameObject(Scene* scene) : 
	_active(true), parentActive(true), _name("GameObject"), _tag(Tag::Default)
{
	if (scene)
		_scene = scene;
	else
	{
		// DEBUG: 오류
		return;
	}

	_transform = new Transform(this);
}

GameObject::GameObject(Scene* scene, bool parentActive) : 
	_active(true), parentActive(parentActive), _name("GameObject"), _tag(Tag::Default)
{
	if (scene)
		_scene = scene;
	else
	{
		// DEBUG: 오류
		return;
	}

	_transform = new Transform(this);
}


GameObject::GameObject(const GameObject& other) 
{
	// 속성들 일단 하나씩 할당...
	// 후에 컴포넌트(포인터)는 Clone 호출(객체 자체에서 또 이걸 반복하는거지)하고서 상호참조부를 this로 교체!
	// Add로 직접 넣어주기 (거의 그냥 생성이다)
	_scene = other._scene;
	_name = other._name + " (Clone)";
	_tag = other._tag;
	_active = other._active;
	parentActive = true;

	_transform = other._transform->CloneTransform();
	_transform->_gameObject = this;

	for (Component* comp : other.pComponentList) 
	{
		Component* clonedComp = comp->Clone();	// TODO..?: 너씨 타입이 뭔데... 주소라서 사실 실행은 되겠으나.. 엔진 선에선 MonoBehavior인지 어떤지만 아니까 보여줄수는 없네

		clonedComp->_gameObject = this;

		pComponentList.push_back(clonedComp);
	}

	// 자식은 복사안함
}


GameObject::~GameObject()
{
	// TODO_LATER: 진짜 삭제보다 모종의 재활용?

	for (Component* comp : pComponentList)
		delete comp;

	delete _transform;
}

#pragma endregion



#pragma region Clone

GameObject* GameObject::Clone() const
{
	return new GameObject(*this);
}

#pragma endregion



#pragma region Properties

Transform* const GameObject::transform()
{
	if (isKilled)		// TODO?: 이거 킬체크 한번쯤 생각은 해야하는데;
	{
		// DEBUG: 죽엇어!
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


void GameObject::InitializeLifecycle(MonoBehavior* behavior)
{
	if (Application::isPlaying())
	{
		if (behavior->activeAwake)
			behavior->Awake();

		if (behavior->activeOnEnable && behavior->isActiveAndEnabled())
			behavior->OnEnable();
	}
	else
	{
		if (behavior->activeAwake)
			DDANZIT_Core::awakeExecQueue.push(behavior);

		if (behavior->activeOnEnable && behavior->isActiveAndEnabled())
			DDANZIT_Core::onEnableExecQueue.push(behavior);
	}

	if (behavior->activeStart)
		DDANZIT_Core::startExecQueue.push(behavior);


	// 활성화 여부에 따라.. 안넣을수도있음
	if (behavior->isActiveAndEnabled())
		DDANZIT_Core::RegisterUpdateExecLists(behavior);
}

#pragma endregion



#pragma region Component

// 기본 컴포넌트 추가 시 특수동작

template <>
SpriteRenderer* GameObject::AddComponent<SpriteRenderer>()
{
	if (pComponentList.size() == MAX_COMPONENT_NUM)
	{
		// DEBUG: 디버그 메세지
		return nullptr;
	}

	Draw2D* draw = dynamic_cast<Draw2D*>(this);

	if (draw == nullptr)
	{
		// DEBUG: 디버그 메세지
		return nullptr;
	}


	SpriteRenderer* spriteRenderer = new SpriteRenderer(this);
	draw->spriteRenderer = spriteRenderer;

	pComponentList.push_back(spriteRenderer);

	return spriteRenderer;
}

template <>
Camera* GameObject::AddComponent<Camera>()
{
	if (pComponentList.size() == MAX_COMPONENT_NUM)
	{
		// DEBUG: 디버그 메세지
		return nullptr;
	}

	Camera* camera = new Camera(this);
	

	pComponentList.push_back(camera);

	return camera;
}

#pragma endregion



#pragma region StaticMethods

GameObject* GameObject::Instantiate(GameObject* gameObject)
{
	if (gameObject == nullptr)
	{
		// DEBUG
		return nullptr;
	}

	if (gameObject->isKilled)
	{
		return nullptr;
	}

	// 메인 씬 루트에 추가

	GameObject* clone = gameObject->Clone();
	clone->_transform->_parent = HIERARCY_ROOT;


	SceneManager::mainScene->hierarchy += clone;		// Register와는 다르다 그냥 있는걸 추가만 함

	for (Component* comp : clone->pComponentList)
	{
		if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(comp))
		{
			clone->InitializeLifecycle(b);
		}
	}

	clone->isInitialized = true;

	return clone;
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

	if (SceneManager::mainScene != parent->_gameObject->_scene)
	{
		// DEBUG: 다른 씬의 오브젝트임
		return nullptr;
	}

	// 부모 자식으로 추가

	GameObject* clone = gameObject->Clone();
	clone->_transform->SetParent(parent);


	SceneManager::mainScene->hierarchy += clone;

	for (Component* comp : clone->pComponentList)
	{
		if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(comp))
		{
			clone->InitializeLifecycle(b);
		}
	}

	clone->isInitialized = true;

	return clone;
}

GameObject* GameObject::Instantiate(GameObject* gameObject, Vector2 position, Vector2 direction)
{
	if (gameObject == nullptr)
	{
		return nullptr;
	}

	if (gameObject->isKilled)
	{
		return nullptr;
	}


	GameObject* clone = gameObject->Clone();
	clone->_transform->_parent = HIERARCY_ROOT;

	clone->_transform->_localPosition = position;
	clone->_transform->_localDirection = direction;

	SceneManager::mainScene->hierarchy += clone;

	for (Component* comp : clone->pComponentList)
	{
		if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(comp))
		{
			clone->InitializeLifecycle(b);
		}
	}

	clone->isInitialized = true;

	return clone;
}

GameObject* GameObject::Instantiate(GameObject* gameObject, Vector2 position, Vector2 direction, Transform* parent)
{
	if (gameObject == nullptr)
	{
		return nullptr;
	}

	if (gameObject->isKilled)
	{
		return nullptr;
	}


	GameObject* clone = gameObject->Clone();
	clone->_transform->SetParent(parent);

	// 월드 기준이라 할거 없음 오예
	clone->_transform->_localPosition = position;
	clone->_transform->_localDirection = direction;

	SceneManager::mainScene->hierarchy += clone;

	for (Component* comp : clone->pComponentList)
	{
		if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(comp))
		{
			clone->InitializeLifecycle(b);
		}
	}

	clone->isInitialized = true;

	return clone;

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


	GameObject* clone = gameObject->Clone();
	clone->_transform->_parent = HIERARCY_ROOT;
	clone->_scene = scene;

	scene->hierarchy += clone;

	for (Component* comp : clone->pComponentList)
	{
		if (MonoBehavior* b = dynamic_cast<MonoBehavior*>(comp))
		{
			clone->InitializeLifecycle(b);
		}
	}

	clone->isInitialized = true;

	return clone;
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

	DDANZIT_Core::destroyScheduledQueue.push(gameObject);

	for (Transform* tr : gameObject->_transform->pChildList)
	{
		Destroy(tr->_gameObject);
	}
}

GameObject* GameObject::Find(string name) 
{
	for (Scene* scene : SceneManager::pLoadedSceneList)
	{
		if (GameObject* go = scene->hierarchy.GetObjectByName(name))
			return go;
	}

	return nullptr;
}

GameObject* GameObject::FindWithTag(Tag tag) 
{
	for (Scene* scene : SceneManager::pLoadedSceneList)
	{
		if (GameObject* go = scene->hierarchy.GetObjectByTag(tag))
			return go;
	}

	return nullptr;
}

#pragma endregion