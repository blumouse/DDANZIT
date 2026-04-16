#include "GameObject.h"
#include <vector>

GameObject::GameObject() : data(new ObjectData), visual(nullptr), logic(nullptr){
	RegisterObj(this);
}

GameObject::~GameObject() {
	// 할당된거 다 해제
	delete(data);

	if (visual != nullptr)
		delete(visual);

	if (logic != nullptr)
		delete(logic);

	QuitObj(this);
}

GameObject* GameObject::Instantiate() {
	// 동적할당~~
	// 자체적으로도 가지고 있는게 낫나?
	GameObject* newInstance = new GameObject;
	return newInstance;
}

template <typename T> T* GameObject::AddVisual() {
	// 근데 얘가 ObjectVisual을 상속하는걸 어케 보장하지?
	visual = new T();

	return visual;
}

template <typename T> T* GameObject::AddLogic() {
	logic = new T();

	return logic;
}


void GameObject::Destroy(GameObject* instance) {
	// instance를 참조하고있는 다른 객체가 있다면 다 통보를 또 해줘야되겠는데? ..그런 경우가 있겠지?
	delete(instance);
}

void GameObject::RegisterObj(GameObject* pGameObject) {
	GetObjList().push_back(pGameObject);
}

void GameObject::QuitObj(GameObject* pGameObject) {
	// 찾아서 지워!
}

std::vector<GameObject*> GameObject::GetObjList() {
	static std::vector<GameObject*> objList;		// 한번 생성되면 유지된대
	return objList;
}

// 얘네들 다 널체크 해주자
void GameObject::Init() {
	// 아직 여기서 생각할건 아니지만..
	// 한번만 실행이니까 이런건 음 오브젝트의 init을 한번만 가져가서 실행하게 해야겠네
	if (logic != nullptr)
		logic->Init();
	if (visual != nullptr)
		visual->Init();
}

void GameObject::Update() {
	if (logic != nullptr)
		logic->Update();
	if (visual != nullptr)
		visual->Update();
}

void GameObject::LateUpdate() {
	if (logic != nullptr)
		logic->LateUpdate();
	if (visual != nullptr)
		visual->LateUpdate();
}

void GameObject::Close() {
	if (logic != nullptr)
		logic->Close();
	if (visual != nullptr)
		visual->Close();
}