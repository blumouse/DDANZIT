#pragma once
#include "Pipeline.h"
#include "ObjectData.h"
#include "ObjectVisual.h"
#include "ObjectLogic.h"
#include <vector>

class GameObject : Pipeline
{
public:
	ObjectData data;		// 얘네가 간이(고정) 컴포넌트 역할인거지 결국
	ObjectVisual visual;	// 그럼 프레임워크 아래에서 코드를 짤때는
	ObjectLogic logic;		// 얘네를 상속해서 이어서 짜면 될라나

	GameObject();
	~GameObject();

	static GameObject* Instantiate();	// 이런식으로?
	static void Destroy(GameObject* instance);
	static std::vector<GameObject*> GetObjList();
	static GameObject* GetObject(long index);	// 근데 뭘 갖다 찾지
	static GameObject* GetObject(ObjectData data);	// 음.. 컴포넌트로 찾기
	static GameObject* GetObject(ObjectVisual visual);
	static GameObject* GetObject(ObjectLogic logic);
private:
	long index;			// ? 필요하지 않을까
	void RegisterObj(GameObject* pGameObject);
	void taltoe(GameObject* pGameObject);
};

// 아 이거 말고 DI로 가야되나
// 어찌됐건 게임 업데이트 하려면 전부 등록하긴 해야지
// 인터페이스 없고 추상클래스로 대신해야되네 다중상속됨

// ! 업데이트 관련 상속을 여기다 두지 말까? visual이랑 logic에 상속주면 되자나
// 여기도 가질수 있을거같긴 한데.. 암튼 업데이트 클래스를 따로 빼야될듯

// 여기가 헤드니까 헤드끼리 찾고 통신하고 그런 기능이 있어야될듯
// 리스트 갖고있으니까 활용하면 될듯?