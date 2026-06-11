#pragma once
#include "DDANZIT.h"
#include "DDANZITEngine.h"
#include "SceneManagement.h"

#include "CodeEdit.h"
#include "Debug.h"
#include "Utillity.h"

#include "Player.h"
#include "aria.h"
#include "gungwoo.h"
#include "suna.h"

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

	//3명을 담을 Player 추가 
	GameObject* player = scene1->hierarchy.AddGameObject<Player>();
	player->transform()->SetPosition(Vector2f(0, -350));
	player->transform()->SetScale(Vector2f(1.3f, 1.3f));
	player->name() = "Player";
	player->AddComponent<CircleCollider2D>();

	//수나 오브젝트
	GameObject* su = scene1->hierarchy.AddGameObject<suna>();
	SpriteRenderer* sSp = su->AddComponent<SpriteRenderer>();
	//넣을 때  n번 프레임 시작 위치, 크기
	sSp->sprite = SpriteIndex::Player1;
	sSp->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	sSp->TryAddAtlasRect("wa", SpriteAtlasRect(0, 0, 396, 743));
	sSp->TrySetAtlas("wa");

	su->transform()->SetParent(player->transform());
	su->transform()->SetLocalScale(Vector2f(0.8f, 0.8f));
	su->transform()->SetPosition(Vector2f(-70, 0));

	//궁우 오브젝트
	GameObject* woo = scene1->hierarchy.AddGameObject<gungwoo>();
	SpriteRenderer* wSp = woo->AddComponent<SpriteRenderer>();
	//넣을 때  n번 프레임 시작 위치, 크기
	wSp->sprite = SpriteIndex::Player2;
	wSp->flipX = true;
	wSp->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	wSp->TryAddAtlasRect("wa", SpriteAtlasRect(5, 1, 361, 640));
	wSp->TrySetAtlas("wa");

	woo->transform()->SetParent(player->transform());
	woo->transform()->SetPosition(Vector2f(70, -10));

	
	//지울것
	BeatMediaSystem::Instance().PlayMp3(MusicIndex::kanden,true);	
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