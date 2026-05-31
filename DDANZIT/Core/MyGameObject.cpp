#include "MyGameObject.h"
#include "Game.h"
#include "Component.h"


GameObjectBase::GameObjectBase() 
{
	ppComponents = new Component* [MAX_COMPONENT_SIZE];
}

// ..?
GameObjectBase::~GameObjectBase()
{
	for (int i = 0; i < componentsSize; i++)
	{
		//ppComponents[i]->OnDestroy();
		delete ppComponents[i];
	}
}


void GameObjectBase::Awake() 
{
	for (int i = 0; i < componentsSize; i++)
	{
		ppComponents[i]->Awake();
	}
}

void GameObjectBase::Start()
{
	for (int i = 0; i < componentsSize; i++)
	{
		ppComponents[i]->Start();
	}
}

void GameObjectBase::Update(float deltaTime)
{
	for (int i = 0; i < componentsSize; i++)
	{
		ppComponents[i]->Update(deltaTime);
	}
}

void GameObjectBase::FixedUpdate()
{
	for (int i = 0; i < componentsSize; i++)
	{
		ppComponents[i]->FixedUpdate();
	}
}

void GameObjectBase::OnDestroy()
{
	for (int i = 0; i < componentsSize; i++)
	{
		ppComponents[i]->OnDestroy();
	}
}