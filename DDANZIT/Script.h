#include "MonoBehavior.h"
#include "GameObject.h"

class Script : public MonoBehavior 
{
public:
	Script(GameObject* gameObject) : MonoBehavior(gameObject)
	{
		EnableAwake();
	}

	void Awake() 
	{
		auto comp = gameObject()->GetComponent<Script>();
	}
};