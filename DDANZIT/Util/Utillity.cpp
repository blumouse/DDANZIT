#include "INC_Windows.h"
#include "Utillity.h"

namespace learning
{
	int g_width = 800;
	int g_height = 600;

	void SetScreenSize(int width, int height)
	{
		g_width = width;
		g_height = height;
	}

	void GetScreenSize(int& width, int& height)
	{
		width = g_width;
		height = g_height;
	}
}