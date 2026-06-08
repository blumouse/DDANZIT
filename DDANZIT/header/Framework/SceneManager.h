#pragma once

#include <string>
#include <vector>

class Scene;
class GameObject;


enum class LoadSceneMode
{
	Single,
	Addtive,
};


// 전역 매니저클래스!
class SceneManager
{
	friend class DDANZIT_Core;
	friend class GameObject;

	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend void DDANZIT_Run();
	friend void DDANZIT_Finalize();

#pragma region Properties

private:
	static Scene* mainScene;
	static Scene* dontDestroyOnLoad;

	//static std::vector<const wchar_t*> pScenePathList;
	static std::vector<Scene*> pSceneList;
	static std::vector<Scene*> pSceneInstanceList;	// 이건 Create로 만든거 메모리에만 있음 (위 리스트랑 붙어있는 구조라 생각하자)

	static std::vector<Scene*> pLoadedSceneList;

public:
	static int sceneCount() { return pSceneList.size() + pSceneInstanceList.size(); }

	static int loadedSceneCount() { return pLoadedSceneList.size(); }

#pragma endregion



#pragma region Methods

public:
	static bool SetActiveScene(Scene* scene);
	static Scene* GetActiveScene();

	static Scene* CreateScene(const std::string& name);
	static Scene* CreateScene(const std::string& name, LoadSceneMode mode);	// 이것도 원래 없지만 편의를 위해 추가

	static Scene* GetSceneAt(int index);
	static Scene* GetSceneByName(const std::string& name);

	// 원래는 없다
	static void LoadScene(Scene* scene);
	static void LoadScene(Scene* scene, LoadSceneMode mode);
	static void LoadScene(const std::string& name);
	static void LoadScene(const std::string& name, LoadSceneMode mode);

	//static void LoadSceneAsync(std::string name);
	//static void LoadSceneAsync(std::string name, LoadSceneMode mode);

	static bool UnloadScene(Scene* scene);
	static bool UnloadScene(const std::string& name);

	//static void UnloadSceneAsync(Scene* scene);
	//static void UnloadSceneAsync(std::string name);

	static void MoveGameObjectToScene(GameObject* go, Scene* scene);


	// 에디터를 대신하여... 씬 파일 만들기 (후일)
	//static Scene* AddScene(std::string name);

	// 내부용
private:
	static void DontDestroyOnLoad(GameObject* go);

#pragma endregion

};

