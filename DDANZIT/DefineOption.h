#pragma once

enum class Tag
{
	Default,
	Player,
	Enemy,
	Background,
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


#define MAX_SCENE_GAME_OBJECT_NUM		1000
#define MAX_LAYER_NUM					5
#define MAX_RESOURCE_NUM				10

#define MAX_COMPONENT_NUM				20



//
//
//



#define HIERARCY_ROOT					nullptr