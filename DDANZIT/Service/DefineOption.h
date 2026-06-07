#pragma once


enum class Tag
{
	Default,
	Player,
	Enemy,
	Background,
};

// 이따구로 써도 될까..?
typedef enum class SpriteIndex
{
	None = -1,
	Sqaure = -2,
	Circle = -3,
	Capsule = -4,
	// 스프라이트 리소스는 0번부터 (인덱스)

} Sprite;



//
//
//



#define USE_DEBUG
//#define USE_EDITOR


#define RENDER_MODE_WINGDI
//#define RENDER_MODE_DIRECT2D


#define PROPS_MODE_2D
//#define PROPS_MODE_3D



#define MAX_SCENE_GAME_OBJECT_NUM		2000
#define MAX_LAYER_NUM					20
#define MAX_RESOURCE_NUM				50

#define MAX_COMPONENT_NUM				20



//
//
//
// 여긴 또 분리하는게 낫나?


#define HIERARCY_ROOT					nullptr