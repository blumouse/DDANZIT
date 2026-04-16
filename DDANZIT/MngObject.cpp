#include "GameObject.h"

class NewVisual : ObjectVisual
{
	// 여기에 자체로직을 쓰겠지?
	// 코드 인젝션을 시킬건데 음 걍 업데이트에 넣게 하면 끝인가?


public:
	NewVisual();
	~NewVisual();

	void Update() {
		Myfunc();
		// 어쩌구저쩌구... 써놓으면 이걸 가져가서 실행
	}

private:
	void Myfunc();
};

class MngObject : GameObject
{
private:
	void InitObj();
};

void MngObject::InitObj() {
	// 얘가 직접 만들면 요상하지..?
	// 누군가에게 요청하거나 스스로 만들어져 나오거나 가 되어야돼
	GameObject* myObj = GameObject::Instantiate();

	// NewVisual* v = new NewVisual();
	// myObj->LinkComponent((ObjectVisual*)v);		// 이건 너무 귀찮차나!

	myObj->AddVisual<NewVisual>();

	AddLogic<NewVisual>();
}
