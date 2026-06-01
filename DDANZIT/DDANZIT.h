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



// 이녀석들이 노출된.. 이벤트 등록 역할 (수정 필요)
// 이것도 따로 인풋시스템 같은쪽으로 빼는게 낫겠다
//void OnMouseMove(int x, int y);
//void OnLButtonDown(int x, int y);
//void OnLButtonUp(int x, int y);
//void OnRButtonDown(int x, int y);
//void OnRButtonUp(int x, int y);


// 상위 (유저)에서 호출할거랑 (위)
// 하위 (게임)에서 호출할거랑 (아래)
// 둘다 공개는 해야되지만 또 구분은 해야하는데; 음.. 아래를 클래스로 뺄까
// 어차피 숨긴다 치면 무방하긴 해 최악에는 무적의 friend도 있고..
// 생각해보니까 또 리소스는 그렇다 치는데 이거 씬 / 하이라키로 들어가야 되는거네?


BitmapInfo* LoadResource(const wchar_t* filePath);


//GameObjectBase* GetObjectWithPos(int mouseX, int mouseY);
//bool TryGetObjectWithPos(int mouseX, int mouseY, GameObjectBase*& pGameObject);