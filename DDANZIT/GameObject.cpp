#include "GameObject.h"
#include <vector>

GameObject::GameObject() : data(new ObjectData), visual(nullptr), logic(nullptr){
	RegisterObj(this);
}

GameObject::~GameObject() {
	// 할당된거 다 해제
	delete(data);
	// 왜됨? 값을 변경하는게 아닌가?
	// 적어도 가리키는걸 nullptr로 돌려버리는 정도는.. 직접해야되는가보네

	if (visual != nullptr)
		delete(visual);

	if (logic != nullptr)
		delete(logic);

	QuitObj(this);
}

#pragma region Component

// Not for use now...
template <std::derived_from<ObjectVisual> T> bool GameObject::TryAddVisual(T*& derivedVisual) {
	return false;
}

// Not for use now...
template <std::derived_from<ObjectLogic> T> bool GameObject::TryAddLogic(T*& derivedLogic) {
	return false;
}

#pragma endregion

#pragma region ManageObject

GameObject* GameObject::Instantiate() {
	// 동적할당~~
	// 자체적으로도 가지고 있는게 낫나?
	GameObject* newInstance = new GameObject;
	return newInstance;
}

void GameObject::Instantiate(GameObject& gameObject) {
	GameObject* newInstance = new GameObject;
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

#pragma endregion

#pragma region Pipeline

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

#pragma endregion