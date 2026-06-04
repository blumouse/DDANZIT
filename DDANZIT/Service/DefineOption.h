#pragma once


enum class Tag
{
	Default,
	Player,
	Enemy,
	Background,
};

// 이따구로 써도 될까..?
enum class SpriteIndex
{
	None = -1,
};



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



#define HIERARCY_ROOT					nullptr