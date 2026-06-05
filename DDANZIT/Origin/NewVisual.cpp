#include "ObjectVisual.h"

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