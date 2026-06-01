#include "GameObject.h"

#include "DDANZIT.h"
#include "Transform.h"
#include "Component.h"
#include "Scene.h"

// TODO: MAX_...어쩌고를 인덱스 넘으려고 하면 방어를 해주고싶은데
// 그냥 팩토리면 방어되나? new를 가리고 friend로 빼주고서..
// ...강제는 못하겠고 차피 뭐 게임에서 돌아갈라면 등록을 해야되니까
// 컴포넌트도 맡기는 함수가 있어야겠다 암튼 (결국)
// 하이라키에 등록하거나 Instantiate하거나 하면 그후에 내부에서 등록.. 할때!
// 아닌가 굳이 방어할거 없나..? 이건 너무 예외처리 영역인데 일단 킵

using namespace std;


#pragma region Constructor

GameObject::GameObject() : _active(true), _tag(Tag::Default)
{
	_transform = new Transform(this);
}

GameObject::~GameObject()
{
	for (Component* comp : pComponentList)
		delete comp;
}

#pragma endregion



#pragma region Properties

Transform* const GameObject::transform()
{
	return _transform;
}

#pragma endregion



#pragma region Methods

void GameObject::SetActive(bool newActive) 
{
	if (_active == newActive)
		return;

	if (newActive /*== true*/)
	{
		// TODO: OnEnable 리스트에 추가
	}
	else
	{
		// 유사
	}

	_active = newActive;
}

#pragma endregion



#pragma region LifeCycles

void GameObject::Awake()
{
	for (Component* comp : pComponentList)
		comp->Awake();
}

void GameObject::OnEnable()
{

}

void GameObject::Start()
{
	for (Component* comp : pComponentList)
		comp->Start();
}

void GameObject::Update()
{
	for (Component* comp : pComponentList)
		comp->Update();
}

void GameObject::FixedUpdate()
{
	for (Component* comp : pComponentList)
		comp->FixedUpdate();
}

void GameObject::OnDestroy()
{
	for (Component* comp : pComponentList)
		comp->OnDestroy();
}

#pragma endregion



#pragma region StaticMethods

GameObject* GameObject::Instantiate(GameObject* gameObject)
{
	if (gameObject == nullptr)
		return nullptr;

}

static GameObject* Instantiate(GameObject* gameObject, Transform* parent) 
{
	if (gameObject == nullptr)
		return nullptr;

}

static GameObject* Instantiate(GameObject* gameObject, Vector2 position, float angle) 
{
	if (gameObject == nullptr)
		return nullptr;

}

static GameObject* Instantiate(GameObject* gameObject, Scene scene)
{
	if (gameObject == nullptr)
		return nullptr;

}

void GameObject::Destroy(GameObject*& gameObject) 
{
	if (gameObject == nullptr)
		return;

}

GameObject* GameObject::Find(string name) 
{

}

GameObject* GameObject::FindWithTag(Tag tag) 
{

}

#pragma endregion