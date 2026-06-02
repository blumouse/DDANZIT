#include "Scene.h"

#include "GameObject.h"

using namespace std;


#pragma region Hierarchy

vector<GameObject*> Scene::GetRootGameObjects() const 
{

}

const int& Scene::rootCount() const 
{

}

#pragma endregion



void Scene::RegisterGameObject(GameObject* gameObject)
{

}

void Scene::QuitGameObject(GameObject* gameObject)
{

}


void Scene::RegisterDrawable(IDrawable* drawable)
{

}

void Scene::RegisterDrawable(IDrawable* drawable, int layer)
{

}

void Scene::QuitDrawable(IDrawable* drawable)
{

}

void Scene::QuitDrawable(IDrawable* drawable, int layer)
{

}