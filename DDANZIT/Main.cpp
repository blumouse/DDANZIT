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

#pragma region Player
	//3명을 담을 Player 추가 
	GameObject* player = scene1->hierarchy.AddGameObject<Player>();
	player->transform()->SetPosition(Vector2f(0, -350));
	player->transform()->SetScale(Vector2f(1.3f, 1.3f));
	player->name() = "Player";
	player->AddComponent<CircleCollider2D>();

	//아리아 오브젝트
	GameObject* ariObj = scene1->hierarchy.AddGameObject<aria>();
	SpriteRenderer* aSp = ariObj->AddComponent<SpriteRenderer>();
	ariObj->name() = "Aria";
	//넣을 때  n번 프레임 시작 위치, 크기
	aSp->useAtlas = true;
	aSp->sprite = SpriteIndex::aria;
	aSp->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	aSp->TryAddAtlasRect("idle", SpriteAtlasRect(0, 0, 300, 750));
	aSp->TrySetAtlas("idle");

	ariObj->transform()->SetParent(player->transform());
	ariObj->transform()->SetLocalScale(Vector2f(1.f, 1.f));

	//수나 오브젝트
	GameObject* suObj = scene1->hierarchy.AddGameObject<suna>();
	SpriteRenderer* sSp = suObj->AddComponent<SpriteRenderer>();
	//넣을 때  n번 프레임 시작 위치, 크기
	suObj->name() = "Suna";
	sSp->useAtlas = true;
	sSp->sprite = SpriteIndex::suna;
	sSp->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	sSp->TryAddAtlasRect("idle", SpriteAtlasRect(0, 0, 300, 750));
	sSp->TrySetAtlas("idle");

	suObj->transform()->SetParent(player->transform());
	suObj->transform()->SetLocalScale(Vector2f(1.f, 1.f));
	suObj->transform()->localPosition() = Vector2f(-70, 0);

	//궁우 오브젝트
	GameObject* wooObj = scene1->hierarchy.AddGameObject<gungwoo>();
	SpriteRenderer* wSp = wooObj->AddComponent<SpriteRenderer>();
	//넣을 때  n번 프레임 시작 위치, 크기
	wooObj->name() = "GungWoo";
	wSp->useAtlas = true;
	wSp->sprite = SpriteIndex::gung;
	wSp->flipX = true;
	wSp->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	wSp->TryAddAtlasRect("idle", SpriteAtlasRect(0, 0, 300, 750));
	wSp->TrySetAtlas("idle");

	wooObj->transform()->SetParent(player->transform());
	wooObj->transform()->SetLocalScale(Vector2f(1.f, 1.f));
	wooObj->transform()->localPosition() = Vector2f(70, 0);

#pragma endregion



#pragma endregion


	//지울것
	BeatMediaSystem::Instance().PlayMp3(MusicIndex::kanden,true);	
}


int main()
{
	if (!DDANZIT_Initialize(L"Game", 1600, 900))
			return -1;
	if (!DDANZIT_LoadResources(ResourceType::Sprite, filePath, 3))
		return-1;
	if (!DDANZIT_LoadResources(ResourceType::Media, Mp3FilePath, 2,Mp4FilePath,3,SfxFilePath,1))
		return-1;


		
	// 이 사이에 유저가 씬 / 오브젝트 생성 등 게임 빌드 작업...
	GameDataRoading();

	DDANZIT_Run();

	DDANZIT_Finalize();

	return 0;
}