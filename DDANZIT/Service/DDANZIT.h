#pragma once



// 여기엔 접근 가능한 것들 선언


bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize);


void DDANZIT_Run();


void DDANZIT_Finalize();
