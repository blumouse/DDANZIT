#include "SceneManager.h"

#include "DefineOption.h"

#include "Scene.h"
#include "GameObject.h"

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
	Scene* newScene = new Scene(name);

	pSceneInstanceList.push_back(newScene);

	LoadScene(newScene, LoadSceneMode::Addtive);

	return newScene;
}

Scene* SceneManager::CreateScene(const string& name, LoadSceneMode mode)
{
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
	}
}

void SceneManager::LoadScene(const string& name)
{
	Scene* scene = GetSceneByName(name);

	if (scene == nullptr)
	{
		// DEBUG: 그없
		return;
	}

	LoadScene(scene);
}

void SceneManager::LoadScene(const string& name, LoadSceneMode mode)
{
	Scene* scene = GetSceneByName(name);

	if (scene == nullptr)
	{
		// DEBUG: 그없
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
		// DEBUG: 
		return false;
	}

	// TODO: 로드되지 않은 씬이면 리턴

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

	return true;
}

bool SceneManager::UnloadScene(const string& name)
{
	Scene* scene = GetSceneByName(name);

	if (scene == nullptr)
	{
		// DEBUG: 그없
		return false;
	}

	return UnloadScene(scene);
}


//static void UnloadSceneAsync(Scene* scene)

//static void UnloadSceneAsync(std::string name)

//static void UnloadSceneAsync(std::string name, LoadSceneMode mode)


void SceneManager::MoveGameObjectToScene(GameObject* go, Scene* scene)
{
	// DEBUG: 이거 로드 안된 씬이면.. 그냥 에러를 뿜고 중단하나보군
	go->_scene->RemoveFromHierarchy(go);

	scene->AddToHierarchy(go, HIERARCY_ROOT);
}



//static Scene* AddScene(std::string name)



void SceneManager::DontDestroyOnLoad(GameObject* go)
{
	if (go == nullptr || go->isKilled)
	{
		// DEBUG: 그없
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
