#include "GameObject.h"

class MngObject : GameObject
{
private:
	void InitObj();
};

void MngObject::InitObj() {
	// 얘가 직접 만들면 요상하지..?
	// 누군가에게 요청하거나 스스로 만들어져 나오거나 가 되어야돼
	GameObject* myObj = GameObject::Instantiate();
}
