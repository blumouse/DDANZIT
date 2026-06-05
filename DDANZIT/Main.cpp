#include "DDANZIT.h"
#include "SceneManagement.h"
#include "CodeEdit.h"

#include "MyObject.h"


int main()
{
	if (!DDANZIT_Initialize(L"Game", 1024, 760))
		return;


	// 이 사이에 유저가 씬 / 오브젝트 생성 등 게임 빌드 작업...
	Mycode();



	DDANZIT_Run();

	DDANZIT_Finalize();
}


void Mycode() {

	// 에디터 / 직렬화가 없기 때문에 무조건 전부 동적생성;

	Scene* scene1 = SceneManager::CreateScene("Scene1");

	GameObject* go = scene1->hierarchy.RegisterGameObject<MyObject>();



	hierarchy()->GetObjectList();
}