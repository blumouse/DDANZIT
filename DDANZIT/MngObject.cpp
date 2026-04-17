#include "GameObject.h"

class NewVisual : public ObjectVisual
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

class MngObject : GameObject	// 상속받지 못하게 해야겠다 못한다기보다 의미가 없는거로..
{
private:
	void InitObj();
};

void MngObject::InitObj() {
	// 얘가 직접 만들면 요상하지..?
	// 누군가에게 요청하거나 스스로 만들어져 나오거나 가 되어야돼
	GameObject* myObj = GameObject::Instantiate();	// 흠 포인터는 안이뻐 이것도 타입정의 해버릴까

	// NewVisual* v = new NewVisual();
	// myObj->LinkComponent((ObjectVisual*)v);		// 이건 너무 귀찮차나!

	myObj->AddVisual<NewVisual>();
}
