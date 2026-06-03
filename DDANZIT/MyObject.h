#include "GameObject.h"
#include "Script.h"

// 사실 원래는 이렇게 상속받지 않지만;
class MyObject : public GameObject
{
public:
	MyObject(Scene* scene) 
	{
		AddComponent<Script>();

		// 씬/하이라키 에 추가
	}

	~MyObject() = default;

private:

};