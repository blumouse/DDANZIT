#include "GameObject.h"
#include "Script.h"

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