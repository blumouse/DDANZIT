#pragma once


class GameTimer;
class GameObjectBase;
class IDrawable;
class Transform;

namespace renderHelp
{
    class BitmapInfo;
}

using BitmapInfo = renderHelp::BitmapInfo;


// 여기엔 접근 가능한 것들 선언


static bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
static bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize);

static void DDANZIT_Run();

static void DDANZIT_Finalize();


BitmapInfo* LoadResource(const wchar_t* filePath);


void RegisterObject(GameObjectBase* gameObject);

void RegisterDrawable(IDrawable* drawable);
void RegisterDrawable(IDrawable* drawable, int layer);
void QuitDrawable(IDrawable* drawable);
void QuitDrawable(IDrawable* drawable, int layer);

void Destroy(GameObjectBase* gameObject);


//GameObjectBase* GetObjectWithPos(int mouseX, int mouseY);
//bool TryGetObjectWithPos(int mouseX, int mouseY, GameObjectBase*& pGameObject);



// 이녀석들이 노출된.. 이벤트 등록 역할 (수정 필요)
// 이것도 따로 인풋시스템 같은쪽으로 빼는게 낫겠다
//void OnMouseMove(int x, int y);
//void OnLButtonDown(int x, int y);
//void OnLButtonUp(int x, int y);
//void OnRButtonDown(int x, int y);
//void OnRButtonUp(int x, int y);