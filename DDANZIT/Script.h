#include "MonoBehavior.h"
#include "GameObject.h"

class Script : public MonoBehavior 
{
public:
	Script(GameObject* gameObject) : MonoBehavior(gameObject)
	{
		EnableAwake();
	}

	// 인스턴트 하고싶으면 복사생성자 / 클론 함수 재정의도 하셔야 해요

	void Awake() 
	{
		auto comp = gameObject()->GetComponent<Script>();
	}
};