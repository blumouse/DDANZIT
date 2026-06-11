#pragma once
#include "DDANZIT.h"
#include "DDANZITEngine.h"
#include "SceneManagement.h"

#include "CodeEdit.h"
#include "Debug.h"
#include "Utillity.h"

#include "Player.h"
#include "ResourceFilePath.h"
#include "DefineOption.h"
#include "BeatMediaControl.h"

using namespace learning;


void GameDataRoading() {

#pragma region [Scene 1]

	Scene* scene1 = SceneManager::CreateScene("Scene1");
	scene1->backgroundColor() = Color(0.25f, 0.25f, 0.25f, 1.0f);

	//카메라 추가
	GameObject* cam = hierarchy()->AddGameObject();
	cam->AddComponent<Camera>();

	//플레이어 추가 
	GameObject* go = scene1->hierarchy.AddGameObject<Player>();
	SpriteRenderer* sp = go->AddComponent<SpriteRenderer>();
	go->AddComponent<CircleCollider2D>();
	go->transform()->SetLocalAngle(10.0f);

	sp->sprite = SpriteIndex::Player2;
	sp->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	go->transform()->SetPosition(Vector2f(0, -400));
	go->transform()->SetScale(Vector2f(1.3f, 1.3f));

	go->name() = "Player";
	//지울것
	BeatMediaSystem::Instance().PlayMp3(MusicIndex::zzz,true);
	
}


int main()
{
	if (!DDANZIT_Initialize(L"Game", 1600, 900, filePath, 2,Mp3FilePath,2,Mp4FilePath,3,SfxFilePath,1))
		return -1;
	
	// 이 사이에 유저가 씬 / 오브젝트 생성 등 게임 빌드 작업...
	GameDataRoading();

	DDANZIT_Run();

	DDANZIT_Finalize();

	return 0;
}