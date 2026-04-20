#pragma once
#include "Pipeline.h"
#include "ObjectData.h"
#include "ObjectVisual.h"
#include "ObjectLogic.h"
#include "Component.h"
#include <vector>
// 나중에 이것도 한번 정리해야겠다

// 점점 읽기 어려워진닷!!

class GameObject : public Pipeline
{
#pragma region Field

public:

private:
	long index;			// ? 필요하지 않을까

	ObjectData* const data;		// 얘네가 간이(고정) 컴포넌트 역할인거지 결국
	ObjectVisual* visual;	// 그럼 프레임워크 아래에서 코드를 짤때는
	ObjectLogic* logic;		// 얘네를 상속해서 이어서 짜면 될라나
	// 왜 get만 두고싶지
	std::vector<Component*> components;

#pragma endregion


public:
	GameObject();
	~GameObject();

#pragma region Component


	// 뭐야이게
	/*template <std::derived_from<ObjectData> T>
	T* AddComponent() {
		if (data != nullptr)
		{
			delete data;
		}

		data = new T();
		data->gameObject = this;

		return data;
	}*/

	template <std::derived_from<ObjectVisual> T>
	T* AddComponent() {
		if (visual != nullptr)
		{
			delete visual;
		}

		visual = new T();
		visual->gameObject = this;

		return visual;
	}

	template <std::derived_from<ObjectLogic> T>
	T* AddComponent() {
		if (logic != nullptr)
		{
			delete logic;
		}

		logic = new T();
		logic->gameObject = this;

		return logic;
	}

	// 뉴타입 컴포넌트는 명시적으로 분리
	template <std::derived_from<Component> T>
	void RegisterComponent(T* component) {
		components.push_back(component);
	}

	template <std::derived_from<Component> T>
	T* GetComponent() {
		// 컴포넌트 찾아!
	}

	// 같은 컴포넌트를 여러개 달수 있어야 하나..?

	// template <std::derived_from<ObjectVisual> T> bool TryAddVisual(T*& derivedVisual);
	// template <std::derived_from<ObjectLogic> T> bool TryAddLogic(T*& derivedLogic);

#pragma endregion


#pragma region ManageObject

	static GameObject* Instantiate();	// 이런식으로?
	static void Instantiate(GameObject& gameObject);
	static void Destroy(GameObject* instance);
	static std::vector<GameObject*> GetObjList();
	static GameObject* GetObject(long index);			// 근데 뭘 갖다 찾지
	static GameObject* GetObject(ObjectData* data);		// 음.. 컴포넌트로 찾기
	static GameObject* GetObject(ObjectVisual* visual);
	static GameObject* GetObject(ObjectLogic* logic);

#pragma endregion


#pragma region Pipeline

	void Init()	override;
	void Update() override;
	void LateUpdate() override;
	void Close() override;

#pragma endregion


private:
	void RegisterObj(GameObject* pGameObject);
	void QuitObj(GameObject* pGameObject);
};

// 아 이거 말고 DI로 가야되나
// 어찌됐건 게임 업데이트 하려면 전부 등록하긴 해야지
// 인터페이스 없고 추상클래스로 대신해야되네 다중상속됨

// ! 업데이트 관련 상속을 여기다 두지 말까? visual이랑 logic에 상속주면 되자나
// 여기도 가질수 있을거같긴 한데.. 암튼 업데이트 클래스를 따로 빼야될듯

// 여기가 헤드니까 헤드끼리 찾고 통신하고 그런 기능이 있어야될듯
// 리스트 갖고있으니까 활용하면 될듯?
// (사실 이게 있어도 직접 주소받아다 쓰는게 낫다)

// 로직쪽 둘은 기본은 널상태로 두고 쓸때 만들어서 쓰기
// 어떤식으로 해보까.. 뭐가 사용하기 쉬울까를 생각해봐!
// 1 유저가 만든 상속객체를 직접 가리키(게 하)기
// 2 자체로 새로 할당하고(리퀘스트화) 유저가 만든걸 연결하기 <-가 함수형태라 (내기준) 쉬운듯?
// 사실 둘다 그게 그거일지도 일단 해보자

// 오브젝트끼리의 통신이랑 모듈 추가 쪽으로도 생각해보자
// 원래 쓰던 방식을 참고를 좀더 해보고싶은데.. 코드를 봐야겠어
// 아직 뭔가뭔가 부족함 좀더 사용도 해보고...
// 다른 객체를 가져와서 함수 호출 -> 이 통신
// 근데 이걸 뭐 따로 만들수 있나..? 걍 함수호출이지
// -> 정말 통신 로직을 참고해보는것도 엔드포인트 만들고 포트연결 해놓고...
// 모듈 추가는 음 일단 입력쪽 기능 모듈로 만들면서 해보자