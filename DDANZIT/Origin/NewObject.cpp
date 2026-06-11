#include "GameObject.h"

class NewObject : GameObject
{
public:
	float newX;
	float newY;
	void Boo();

	NewObject() {
		newX = data.pos.XPos;
		newY = data.pos.YPos;
	}

	void Update() {

	}
};
