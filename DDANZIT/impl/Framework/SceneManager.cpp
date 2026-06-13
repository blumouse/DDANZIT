#include "SceneManager.h"

#include "DefineOption.h"

#include "Scene.h"
#include "GameObject.h"

#include "Debug.h"


using namespace std;


#pragma region Properties

Scene* SceneManager::mainScene = nullptr;
Scene* SceneManager::dontDestroyOnLoad = nullptr;

vector<Scene*> SceneManager::pSceneList;
vector<Scene*> SceneManager::pSceneInstanceList;

vector<Scene*> SceneManager::pLoadedSceneList;

#pragma endregion



#pragma region Methods

bool SceneManager::SetActiveScene(Scene* scene) 
{
	if (scene == nullptr || !scene->isLoaded || mainScene == scene)
		return false;

	mainScene = scene;

	return true;
}

Scene* SceneManager::GetActiveScene() 
{ 
	return mainScene; 
}


Scene* SceneManager::CreateScene(const string& name)
{
	if (GetSceneByName(name))
	{
		Debug::Log("CreateScene: 이미 존재하는 씬 이름입니다.");
		return nullptr;
	}

	Scene* newScene = new Scene(name);

	pSceneInstanceList.push_back(newScene);

	LoadScene(newScene, LoadSceneMode::Addtive);

	return newScene;
}

Scene* SceneManager::CreateScene(const string& name, LoadSceneMode mode)
{
	if (GetSceneByName(name))
	{
		Debug::Log("CreateScene: 이미 존재하는 씬 이름입니다.");
		return nullptr;
	}

	Scene* newScene = new Scene(name);

	pSceneInstanceList.push_back(newScene);

	LoadScene(newScene, mode);

	return newScene;
}


Scene* SceneManager::GetSceneAt(int index)
{
	if (index < 0 || index >= sceneCount())
	{
		// DEBUG: out of range
		return nullptr;
	}

	if (index >= pSceneList.size())
		return pSceneInstanceList[index - pSceneList.size()];
	else
		return pSceneList[index];
}

Scene* SceneManager::GetSceneByName(const string& name)
{
	for (Scene* s : pSceneList)
	{
		if (s->_name == name)
			return s;
	}
	for (Scene* s : pSceneInstanceList)
	{
		if (s->_name == name)
			return s;
	}

	// 없어
	return nullptr;
}


void SceneManager::LoadScene(Scene* scene)
{
	// TODO: 이 시점에 게임오브젝트들이 만들어진다 ...
	// 원래는 파일로 있는거만 되는데 여건상 그냥 한다

	for (Scene* scene : pLoadedSceneList)
		UnloadScene(scene);

	mainScene = scene;

	// 어쩌구저쩌구

	pLoadedSceneList.push_back(scene);
	scene->isLoaded = true;

#ifdef USE_DEBUG_TUI
	Debug::ChangedSceneInfo();

#endif // USE_DEBUG_TUI

}

void SceneManager::LoadScene(Scene* scene, LoadSceneMode mode)
{
	if (mode == LoadSceneMode::Single)
		LoadScene(scene);
	else /*(mode == LoadSceneMode::Addtive)*/
	{
		if (mainScene == nullptr)
			mainScene = scene;


		pLoadedSceneList.push_back(scene);
		mainScene->isLoaded = true;

#ifdef USE_DEBUG_TUI
		Debug::ChangedSceneInfo();

#endif // USE_DEBUG_TUI
	}
}

void SceneManager::LoadScene(const string& name)
{
	Scene* scene = GetSceneByName(name);

	if (scene == nullptr)
	{
		Debug::Assert(false, "LoadScene: 존재하지 않는 씬 이름입니다.");
		return;
	}

	LoadScene(scene);
}

void SceneManager::LoadScene(const string& name, LoadSceneMode mode)
{
	Scene* scene = GetSceneByName(name);

	if (scene == nullptr)
	{
		Debug::Assert(false, "LoadScene: 존재하지 않는 씬 이름입니다.");
		return;
	}

	LoadScene(scene, mode);
}


//static void LoadSceneAsync(std::string name)

//static void LoadSceneAsync(std::string name, LoadSceneMode mode)


// 이거 유니티에서는 [사용되지 않음] ..라네
bool SceneManager::UnloadScene(Scene* scene)
{
	if (scene == nullptr)
	{
		Debug::Assert(false, "UnloadScene: 씬이 nullptr입니다.");
		return false;
	}

	// TODO: 로드되지 않은 씬이면 리턴

	for (Scene* ldScene : pLoadedSceneList)
	{
		if (ldScene == scene)
			break;

		return false;
	}

	// 하이라키의 모든 오브젝트를 파괴한다!
	// 루트만 지우면 재귀적으로 다 없어짐
	for (GameObject* go : scene->GetRootGameObjects())
	{
		GameObject::Destroy(go);
	}

	pLoadedSceneList.erase(remove(
		pLoadedSceneList.begin(),
		pLoadedSceneList.end(), scene),
		pLoadedSceneList.end());

	scene->isLoaded = false;

#ifdef USE_DEBUG_TUI
	Debug::ChangedSceneInfo();

#endif // USE_DEBUG_TUI

	return true;
}

bool SceneManager::UnloadScene(const string& name)
{
	Scene* scene = GetSceneByName(name);

	if (scene == nullptr)
	{
		Debug::Assert(false, "UnloadScene: 씬이 nullptr입니다.");
		return false;
	}

	return UnloadScene(scene);
}


//static void UnloadSceneAsync(Scene* scene)

//static void UnloadSceneAsync(std::string name)

//static void UnloadSceneAsync(std::string name, LoadSceneMode mode)


void SceneManager::MoveGameObjectToScene(GameObject* go, Scene* scene)
{
	if (scene == nullptr)
	{
		Debug::Assert(false, "MoveGameObjectToScene: 씬이 nullptr입니다.");
		return;
	}

	for (Scene* ldScene : pLoadedSceneList)
	{
		if (ldScene == scene)
		{
			go->_scene->RemoveFromHierarchy(go);

			scene->AddToHierarchy(go, HIERARCY_ROOT);
		}
	}


	Debug::Assert(false, "MoveGameObjectToScene: 로드되지 않았거나 존재하지 않는 씬입니다.");
}



//static Scene* AddScene(std::string name)



void SceneManager::DontDestroyOnLoad(GameObject* go)
{
	if (go == nullptr || go->isKilled)
	{
		Debug::Log("DontDestroyOnLoad: 파괴되었거나 존재하지 않는 오브젝트입니다.");
		return;
	}

	if (dontDestroyOnLoad == nullptr)
	{
		dontDestroyOnLoad = new Scene("DontDestroyOnLoad");
		LoadScene(dontDestroyOnLoad, LoadSceneMode::Addtive);
	}

	MoveGameObjectToScene(go, dontDestroyOnLoad);
}

#pragma endregion
