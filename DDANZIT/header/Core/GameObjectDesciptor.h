#pragma once

#include "DefineOption.h"

#include <string>
#include <vector>

class Transform;

// ³Ê¹« ºý¼¾µ¥...?? ½´ÆÛ Åµ
struct ComponentDesciptor
{

};


struct GameObjectDesciptor
{
	std::string name;
	Tag tag;

	// Transform
#ifdef PROPS_MODE_2D
	float posX, posY;
	float scaleX, scaleY;
	float rotationX, rotationY;
	unsigned int depth;
#endif

	int parentIndex = -1;

	std::vector<ComponentDesciptor> componentList;
};


// ³ªÁß¿£ ¾Àµµ ÀÖ¾î¾ßÇÔ