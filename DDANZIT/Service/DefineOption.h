#pragma once


enum class Tag
{
	Default,
	Player,
	Enemy,
	Background,
};


typedef enum class FontIndex
{
	None = -1,
	Default = 0, //UI 텍스트가 사용할 기본 폰트 슬롯입니다.

} Font;

// 이따구로 써도 될까..?
typedef enum class SpriteIndex
{
	None = -1,
	Sqaure = -2,
	Circle = -3,
	Capsule = -4,
	// 스프라이트 리소스는 0번부터 (인덱스)

	suna = 0,
	gung = 1,
	aria = 2,
	enemy = 3, //수정완 프로젝트의 적 스프라이트입니다.
	attackRange = 4, //방향 입력 공격 범위 표시입니다.
	block = 5, //스페이스 가드 입력 표시입니다.
	attacked_sp = 6, //플레이어가 막아야 하는 가드 공격 스프라이트입니다.
	yellowLight = 7, //접근 경고등입니다.
	redLight = 8, //빠른 접근/가드 공격 경고등입니다.
	enemyWalk = 9,
	menuStatic = 10,
	zzzPanel = 11,
	kandenPanel = 12,
	parry = 13,
	blueLight = 14,
	gameBackground = 15,
	ultiCommand = 16,
	HoYoHoYoPanel = 17,
	PixelGalaxyPanel = 18,
	SABMGPanel = 19,
	TripleCounterPanel = 20,
	YoruNoOdorikoPanel = 21,
	
} Sprite;


typedef enum class MusicIndex
{
	None = -1,
	// 리소스는 0번부터 (인덱스)
	zzz = 0,
	kanden = 1,
	choseMusic = 2,
	zzzHighlight = 3,
	kandenHighlight = 4,
	HoYoHoYo = 5,
	PixelGalaxy = 6,
	SABMG = 7,
	TripleCounter = 8,
	YoruNoOdoriko = 9,
	HoYoHoYoHighlight = 10,
	PixelGalaxyHighlight = 11,
	SABMGHighlight = 12,
	TripleCounterHighlight = 13,
	YoruNoOdorikoHighlight = 14,

} Music;

typedef enum class VideoIndex
{
	None = -1,
	//리소스는 0번부터 (인덱스)
	Suna = 0,
	Aria = 1,
	GungWoo = 2,

} Video;


typedef enum class SFXIndex
{
	None = -1,
	//리소스는 0번부터 (인덱스)
	hit = 0,
	televisionStatic = 1,
	tvUiMoved = 2,
	alarm = 3,
	ariaUltimateFinish = 4,
	sunaUltimateFinish = 5,
	wooUltimateFinish = 6,
	parry = 7,
	aodThreeHitsMix = 8,

} SFX;


//
//
//



#define USE_DEBUG
//#define USE_DEBUG_TUI
//#define USE_EDITOR


//#define RENDER_MODE_WINGDI
#define RENDER_MODE_DIRECT2D


#define PROPS_MODE_2D
//#define PROPS_MODE_3D



#define MAX_SCENE_GAME_OBJECT_NUM		2000
#define MAX_LAYER_NUM					20
#define MAX_RESOURCE_NUM				50

#define MAX_COMPONENT_NUM				20


#define FRAME_LATE						60


//
//
//
// 여긴 또 분리하는게 낫나?


#define HIERARCY_ROOT					nullptr
