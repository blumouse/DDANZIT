#include "GameObject.h"
#include <vector>

GameObject::GameObject() : data(new ObjectData(this)), visual(nullptr), logic(nullptr){
	RegisterObj(this);
}

GameObject::~GameObject() {
	// 할당된거 다 해제
	if (data != nullptr)
		delete data;

	if (visual != nullptr)
		delete visual;

	if (logic != nullptr)
		delete logic;

	QuitObj(this);
}

#pragma region Component

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
	delete instance;
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
		logic->Init();		// 흠 이거 함수포인터로 받은뒤에 조건걸어서 거를수도 있겠는데?
	if (visual != nullptr)
		visual->Init();
	// TODO: 확장 컴포넌트도 추가...
	// 파이프라인을 상속하고, 함수포인터 변수에 할당이 돼있으면 그걸 실행
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