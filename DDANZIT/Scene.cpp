#include "Scene.h"

#include "GameObject.h"

// TODO: 큐로 바꾸기


void Scene::RegisterObject(GameObject* gameObject)
{
    // TODO: 오브젝트 관리 / 검색 방식 강화 및 다양화

    for (int i = 0; i < gameObjectsIndex; i++)
    {
        if (ppGameObjects[i] == nullptr)
        {
            ppGameObjects[i] = gameObject;

            // TODO: Awake / Start 실행 큐에 등록
            gameObject->Awake();

            return;
        }
    }


    if (gameObjectsIndex == MAX_GAME_OBJECT_NUM)
        return;

    ppGameObjects[gameObjectsIndex++] = gameObject;

    // TODO: 동일
    gameObject->Awake();
}


void Scene::RegisterDrawable(IDrawable* drawable)
{
    for (int i = 0; i < gameObjectsIndex; i++)
    {
        if (ppDrawableLayers[0][i] == nullptr)
        {
            ppDrawableLayers[0][i] = drawable;
            break;
        }
    }
}

void Scene::RegisterDrawable(IDrawable* drawable, int layer)
{
    if (layer < 0 || layer > MAX_LAYER_NUM)
        return;

    for (int i = 0; i < gameObjectsIndex; i++)
    {
        if (ppDrawableLayers[layer][i] == nullptr)
        {
            ppDrawableLayers[layer][i] = drawable;
            break;
        }
    }
}

void Scene::QuitDrawable(IDrawable* drawable)
{
    for (int i = 0; i < MAX_LAYER_NUM; i++)
    {
        for (int j = 0; j < gameObjectsIndex; j++)
        {
            if (ppDrawableLayers[i][j] && ppDrawableLayers[i][j] == drawable)
            {
                ppDrawableLayers[i] = nullptr;
                break;
            }
        }
    }
}

void Scene::QuitDrawable(IDrawable* drawable, int layer)
{
    for (int i = 0; i < gameObjectsIndex; i++)
    {
        if (ppDrawableLayers[layer][i] && ppDrawableLayers[layer][i] == drawable)
        {
            ppDrawableLayers[layer] = nullptr;
            break;
        }
    }
}

void Scene::Destroy(GameObject* gameObject)
{
    IDrawable* drawable = dynamic_cast<IDrawable*>(gameObject);
    if (drawable)
        QuitDrawable(drawable, drawable->GetLayer());

    for (int i = 0; i < gameObjectsIndex; i++)
    {
        if (ppGameObjects[i] && ppGameObjects[i] == gameObject)
        {
            ppGameObjects[i] = nullptr;

            gameObject->OnDestroy();
            delete gameObject;

            return;
        }
    }
}

