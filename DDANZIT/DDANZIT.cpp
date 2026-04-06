#include <iostream>
#include "GameObject.h"

int main()
{
	while (true)
	{
		GameObject::GetObjList().front()->Update();
		// 순회로 해야겠지
	}
}

// update를 어케 가져와볼까