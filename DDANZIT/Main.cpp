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

#include "Game/UI/HealthBarObject.h" //씬에 미리 배치할 체력바 오브젝트입니다.
#include "Game/Effect/RhythmEffectObject.h" //경고등/타격 파편 이펙트 풀 오브젝트입니다.
#include "Game/Enemy/RhythmEnemyObject.h" //수정완 프로젝트 방식의 적 풀 오브젝트입니다.
#include "Game/Manager/RhythmGameManagerObject.h" //게임 전체 흐름을 관리할 매니저 오브젝트입니다.
#include "Game/GuardAttack/RhythmGuardAttackObject.h" //플레이어가 방어해야 하는 공격 풀 오브젝트입니다.
#include "Game/UI/UiBoxObject.h" //선택 카드와 진행바처럼 텍스트가 없어도 보이는 UI 오브젝트입니다.
#include "Game/UI/UiTextObject.h" //선택/결과 상태를 보여줄 UI 텍스트 오브젝트입니다.

#include <string> //노트 풀 오브젝트 이름을 만들기 위해 사용합니다.

using namespace learning;

SpriteRenderer* ConfigureUiBox(UiBoxObject* object, Vector2f position, Vector2f scale, Color color, SpriteIndex sprite = SpriteIndex::Sqaure) //사전 배치 UI 박스의 공통 초기화입니다.
{
	object->transform()->SetPosition(position);
	object->transform()->SetScale(scale);
	object->transform()->SetDepth(1);

	SpriteRenderer* spriteRenderer = object->GetComponent<SpriteRenderer>();
	spriteRenderer->sprite = sprite;
	spriteRenderer->color = color;

	return spriteRenderer;
}

Text* ConfigureUiText(UiTextObject* object, Vector2f position, Vector2f scale, const wchar_t* value, FontSize fontSize, Color color) //사전 배치 UI 텍스트의 공통 초기화입니다.
{
	object->transform()->SetPosition(position);
	object->transform()->SetScale(scale);
	object->transform()->SetDepth(1);

	Text* text = object->GetComponent<Text>();
	text->font = FontIndex::Default;
	text->fontSize = fontSize;
	text->color = color;
	text->text = value;

	return text;
}


void GameDataRoading() {

#pragma region [Scene 1]

	Scene* scene1 = SceneManager::CreateScene("Scene1");
	scene1->backgroundColor() = Color(0.25f, 0.25f, 0.25f, 1.0f);

	UiBoxObject* gameBackground = scene1->hierarchy.AddGameObject<UiBoxObject>("GameBackground");
	ConfigureUiBox(
		gameBackground,
		Vector2f(0, 0),
		Vector2f(5.0f, 5.0f),
		Color(1.0f, 1.0f, 1.0f, 1.0f),
		SpriteIndex::gameBackground);
	gameBackground->transform()->SetDepth(15);
	gameBackground->transform()->SetScale(Vector2(5.5f,5.5f));
	gameBackground->transform()->SetPosition(Vector2(10,-105.f));
	gameBackground->SetActive(false);

	//카메라 추가
	GameObject* cam = hierarchy()->AddGameObject();
	cam->name() = "MainCamera"; //게임 매니저가 카메라 흔들림 연출을 찾을 수 있게 이름을 붙였습니다.
	cam->AddComponent<Camera>();

#pragma region Player
	//3명을 담을 Player 추가 
	GameObject* player = scene1->hierarchy.AddGameObject<Player>();
	player->transform()->SetPosition(Vector2f(0, -340));
	player->transform()->SetScale(Vector2f(1.3f, 1.3f));
	player->transform()->SetDepth(5);
	player->name() = "Player";
	//Player 내부 생성자에서 Rigidbody/Collider/PlayerController를 직접 붙이도록 정리했습니다.

	//아리아 오브젝트
	GameObject* ariObj = scene1->hierarchy.AddGameObject<aria>();
	SpriteRenderer* aSp = ariObj->AddComponent<SpriteRenderer>();
	ariObj->name() = "Aria";
	ariObj->transform()->SetDepth(5);
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
	suObj->transform()->SetDepth(5);
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
	wooObj->transform()->SetDepth(5);
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

	//본 게임 시작 전 곡/난이도를 고르는 선택 화면 패널입니다.
	UiBoxObject* menuStaticPanel = scene1->hierarchy.AddGameObject<UiBoxObject>("MenuStaticPanel");
	ConfigureUiBox(
		menuStaticPanel,
		Vector2f(0, 0),
		Vector2f(0.96f, 0.96f),
		Color(1.0f, 1.0f, 1.0f, 1.0f),
		SpriteIndex::menuStatic);
	menuStaticPanel->transform()->SetScale(Vector2(5,5));

	UiBoxObject* menuSongPanel = scene1->hierarchy.AddGameObject<UiBoxObject>("MenuSongPanel");
	ConfigureUiBox(
		menuSongPanel,
		Vector2f(0, 0),
		Vector2f(0.96f, 0.96f),
		Color(1.0f, 1.0f, 1.0f, 1.0f),
		SpriteIndex::zzzPanel);
	menuSongPanel->transform()->SetScale(Vector2(5,5));

	ConfigureUiBox(
		scene1->hierarchy.AddGameObject<UiBoxObject>("SongSelectPanel"),
		Vector2f(0, -50),
		Vector2f(12.2f, 5.4f),
		Color(0.01f, 0.01f, 0.01f, 0.26f));

	ConfigureUiText(
		scene1->hierarchy.AddGameObject<UiTextObject>("SongTitleLabel"),
		Vector2f(0, 310),
		Vector2f(900, 52),
		L"CHOOSE SONG",
		FontSize::_32,
		Color(0.96f, 0.98f, 1.0f, 1.0f));

	//곡 선택 카드는 매니저가 선택 상태에 따라 색을 바꿉니다.
	ConfigureUiBox(
		scene1->hierarchy.AddGameObject<UiBoxObject>("SongCard_0"),
		Vector2f(-310, 190),
		Vector2f(4.8f, 1.35f),
		Color(0.16f, 0.19f, 0.24f, 0.88f));
	ConfigureUiText(
		scene1->hierarchy.AddGameObject<UiTextObject>("SongLabel_0"),
		Vector2f(-310, 190),
		Vector2f(420, 68),
		L"[1] zzz",
		FontSize::_28,
		Color(1.0f, 1.0f, 1.0f, 1.0f));

	ConfigureUiBox(
		scene1->hierarchy.AddGameObject<UiBoxObject>("SongCard_1"),
		Vector2f(310, 190),
		Vector2f(4.8f, 1.35f),
		Color(0.16f, 0.19f, 0.24f, 0.88f));
	ConfigureUiText(
		scene1->hierarchy.AddGameObject<UiTextObject>("SongLabel_1"),
		Vector2f(310, 190),
		Vector2f(420, 68),
		L"[2] kanden",
		FontSize::_28,
		Color(1.0f, 1.0f, 1.0f, 1.0f));

	ConfigureUiText(
		scene1->hierarchy.AddGameObject<UiTextObject>("DifficultyTitleLabel"),
		Vector2f(0, 80),
		Vector2f(900, 46),
		L"CHOOSE DIFFICULTY",
		FontSize::_24,
		Color(0.92f, 0.95f, 1.0f, 1.0f));

	//난이도 선택 카드는 Easy/Normal/Hard를 명확하게 보여줍니다.
	ConfigureUiBox(
		scene1->hierarchy.AddGameObject<UiBoxObject>("DifficultyCard_0"),
		Vector2f(-360, 5),
		Vector2f(3.0f, 1.05f),
		Color(0.18f, 0.20f, 0.23f, 0.88f));
	ConfigureUiText(
		scene1->hierarchy.AddGameObject<UiTextObject>("DifficultyLabel_0"),
		Vector2f(-360, 5),
		Vector2f(260, 58),
		L"[3] Easy",
		FontSize::_24,
		Color(1.0f, 1.0f, 1.0f, 1.0f));

	ConfigureUiBox(
		scene1->hierarchy.AddGameObject<UiBoxObject>("DifficultyCard_1"),
		Vector2f(0, 5),
		Vector2f(3.0f, 1.05f),
		Color(0.18f, 0.20f, 0.23f, 0.88f));
	ConfigureUiText(
		scene1->hierarchy.AddGameObject<UiTextObject>("DifficultyLabel_1"),
		Vector2f(0, 5),
		Vector2f(280, 58),
		L"[4] Normal",
		FontSize::_24,
		Color(1.0f, 1.0f, 1.0f, 1.0f));

	ConfigureUiBox(
		scene1->hierarchy.AddGameObject<UiBoxObject>("DifficultyCard_2"),
		Vector2f(360, 5),
		Vector2f(3.0f, 1.05f),
		Color(0.18f, 0.20f, 0.23f, 0.88f));
	ConfigureUiText(
		scene1->hierarchy.AddGameObject<UiTextObject>("DifficultyLabel_2"),
		Vector2f(360, 5),
		Vector2f(260, 58),
		L"[5] Hard",
		FontSize::_24,
		Color(1.0f, 1.0f, 1.0f, 1.0f));

	ConfigureUiBox(
		scene1->hierarchy.AddGameObject<UiBoxObject>("StartHintPanel"),
		Vector2f(0, -120),
		Vector2f(5.6f, 0.76f),
		Color(0.10f, 0.36f, 0.28f, 0.90f));
	ConfigureUiText(
		scene1->hierarchy.AddGameObject<UiTextObject>("StartHintLabel"),
		Vector2f(0, -120),
		Vector2f(520, 48),
		L"SPACE : START",
		FontSize::_24,
		Color(0.95f, 1.0f, 0.92f, 1.0f));

	//게임 UI는 씬에 미리 등록한 뒤 매니저가 필요할 때 값만 갱신합니다.
	UiTextObject* statusTextObj = scene1->hierarchy.AddGameObject<UiTextObject>("StatusText");
	statusTextObj->transform()->SetPosition(Vector2f(0, 400));
	statusTextObj->transform()->SetScale(Vector2f(1180, 70));
	statusTextObj->transform()->SetDepth(1);
	Text* statusText = statusTextObj->GetComponent<Text>();
	statusText->font = FontIndex::Default;
	statusText->fontSize = FontSize::_28;
	statusText->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	statusText->text = L"";

	//결과 텍스트도 미리 올려두고, 게임 종료 상태에서 내용만 갈아끼웁니다.
	UiTextObject* resultTextObj = scene1->hierarchy.AddGameObject<UiTextObject>("ResultText");
	resultTextObj->transform()->SetPosition(Vector2f(0, -215));
	resultTextObj->transform()->SetScale(Vector2f(1700, 300));
	resultTextObj->transform()->SetDepth(1);
	Text* resultText = resultTextObj->GetComponent<Text>();
	resultText->font = FontIndex::Default;
	resultText->fontSize = FontSize::_24;
	resultText->color = Color(0.95f, 0.95f, 0.75f, 1.0f);
	resultText->text = L"";

	//체력바는 SpriteRenderer로 보이는 막대, HealthBarObserver가 PlayerController 체력 콜백을 구독합니다.
	HealthBarObject* healthBar = scene1->hierarchy.AddGameObject<HealthBarObject>("HealthBarFill");
	healthBar->transform()->SetPosition(Vector2f(0, -190));
	healthBar->transform()->SetScale(Vector2f(1.8f, 0.17f));
	healthBar->transform()->SetDepth(1);
	SpriteRenderer* healthBarSprite = healthBar->GetComponent<SpriteRenderer>();
	healthBarSprite->sprite = SpriteIndex::Sqaure;
	healthBarSprite->color = Color(0.25f, 0.95f, 0.35f, 1.0f);

	UiBoxObject* skillGaugeTrack = scene1->hierarchy.AddGameObject<UiBoxObject>("SkillGaugeTrack");
	ConfigureUiBox(skillGaugeTrack, Vector2f(0, -220), Vector2f(1.8f, 0.17f), Color(0.16f, 0.13f, 0.22f, 0.86f));
	skillGaugeTrack->SetActive(false);

	UiBoxObject* skillGaugeFill = scene1->hierarchy.AddGameObject<UiBoxObject>("SkillGaugeFill");
	ConfigureUiBox(skillGaugeFill, Vector2f(0, -220), Vector2f(1.8f, 0.17f), Color(0.55f, 0.38f, 1.0f, 1.0f));
	skillGaugeFill->SetActive(false);

	UiBoxObject* ultiCommand = scene1->hierarchy.AddGameObject<UiBoxObject>("UltiCommand");
	ultiCommand->transform()->SetPosition(Vector2f(0, -30));
	ultiCommand->transform()->SetScale(Vector2f(2.0f, 2.0f));
	ultiCommand->transform()->SetDepth(8);
	SpriteRenderer* ultiCommandSprite = ultiCommand->GetComponent<SpriteRenderer>();
	ultiCommandSprite->useAtlas = true;
	ultiCommandSprite->sprite = SpriteIndex::ultiCommand;
	ultiCommandSprite->currentAtlas = SpriteAtlasRect(0, 0, 690, 216);
	ultiCommandSprite->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	ultiCommand->SetActive(false);

	//플레이 중 실제 진행 여부를 보여주는 음악 진행바입니다.
	UiBoxObject* progressTrack = scene1->hierarchy.AddGameObject<UiBoxObject>("ProgressTrack");
	ConfigureUiBox(progressTrack, Vector2f(0, 420), Vector2f(11.0f, 0.16f), Color(0.14f, 0.16f, 0.20f, 0.86f));
	progressTrack->SetActive(false);

	UiBoxObject* progressFill = scene1->hierarchy.AddGameObject<UiBoxObject>("ProgressFill");
	ConfigureUiBox(progressFill, Vector2f(0, 420), Vector2f(11.0f, 0.16f), Color(0.25f, 0.85f, 1.0f, 1.0f));
	progressFill->SetActive(false);

	

	//수정완 프로젝트의 적 풀입니다. 게임 중 새로 만들지 않고 재사용합니다.
	for (int i = 0; i < 64; ++i)
	{
		RhythmEnemyObject* enemy = scene1->hierarchy.AddGameObject<RhythmEnemyObject>("RhythmEnemy_" + std::to_string(i));
		enemy->transform()->SetPosition(Vector2f(0, 1000));
		enemy->transform()->SetDepth(2 + (i % 3));
		enemy->SetActive(false);
	}

	//스페이스를 누른 상태로 방향 입력해서 막아야 하는 가드 공격 풀입니다.
	for (int i = 0; i < 32; ++i)
	{
		RhythmGuardAttackObject* guardAttack = scene1->hierarchy.AddGameObject<RhythmGuardAttackObject>("RhythmGuardAttack_" + std::to_string(i));
		guardAttack->transform()->SetPosition(Vector2f(0, 1000));
		guardAttack->transform()->SetDepth(2 + (i % 3));
		guardAttack->SetActive(false);
	}

	//경고등/타격 파편/스킬 이펙트도 모두 사전 풀로 운용합니다.
	for (int i = 0; i < 96; ++i)
	{
		RhythmEffectObject* effect = scene1->hierarchy.AddGameObject<RhythmEffectObject>("RhythmEffect_" + std::to_string(i));
		effect->transform()->SetPosition(Vector2f(0, 1000));
		effect->SetActive(false);
	}

	//매니저는 다른 씬 오브젝트가 모두 올라간 뒤 마지막에 생성합니다.
	scene1->hierarchy.AddGameObject<RhythmGameManagerObject>("RhythmGameManager");



#pragma endregion


	//기존 임시 자동 재생은 게임 매니저 상태에서 음악을 시작하도록 막았습니다.
	//BeatMediaSystem::Instance().PlayMp3(MusicIndex::kanden,true);	
}


int main()
{
	if (!DDANZIT_Initialize(L"Game", 1600, 900))
			return -1;
	if (!DDANZIT_LoadResources(ResourceType::Sprite, filePath, 22)) //수정완 플레이에 필요한 적/공격/경고등/배경 이미지까지 로드합니다.
		return-1;
	DDANZIT_LoadResources(ResourceType::Font, FontFilePath, 1); //UI 텍스트용 기본 폰트입니다. 실패해도 렌더러 안전장치가 건너뜁니다.
	if (!DDANZIT_LoadResources(ResourceType::Media, Mp3FilePath, 15,Mp4FilePath,3,SfxFilePath,9))
		return-1;


		
	// 이 사이에 유저가 씬 / 오브젝트 생성 등 게임 빌드 작업...
	GameDataRoading();

	DDANZIT_Run();

	DDANZIT_Finalize();

	return 0;
}
