#include "GameObject.h"
#include "Script.h"

// 사실 원래는 이렇게 상속받지 않지만;
class MyObject : public GameObject
{
public:
	MyObject(Scene* scene) 
	{
		AddComponent<Script>();
	}

	// 런타임, 인스턴트는 아래 복사생성자로 만들어짐
	// 인스턴트하려면 복사생성자랑 클론도 만드세요
	MyObject(const MyObject& other) = default;

	GameObject* Clone() const override 
	{
		return new MyObject(*this);
	}

	~MyObject() = default;

private:

};