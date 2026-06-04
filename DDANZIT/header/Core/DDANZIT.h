#pragma once


// 여기엔 접근 가능한 것들 선언



bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize);

// TODO: 이 중간쯤에 씬을 먼저 만들게 해야하네
// 일단 걍 저 생성자로 받는게 맞는거도 같고 없으면 디폴트 씬 하나 만들어주고
// 생각해보니까 하이라키에 넣을라 해도 오브젝트는 뉴를 해서 넣어야되자나 이건?
// 저 재정의한 오브젝트는 프리팹일 뿐임(대강)

static void DDANZIT_Run();

static void DDANZIT_Finalize();
