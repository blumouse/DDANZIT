#include "CodeEdit.h"

#include <string>

#include "Debug.h"

#include "SceneManager.h"
#include "Scene.h"
#include "Hierarchy.h"

using namespace std;


namespace CodeEdit
{
	// 매번 씬 칠라면 번거롭자나
	Hierarchy* hierarchy()
	{
		if (SceneManager::GetActiveScene() == nullptr)
		{
			Debug::Assert(false, "Hierarchy: 메인 씬이 없습니다.");
			return nullptr;
		}

		return &SceneManager::GetActiveScene()->hierarchy;
	}
}