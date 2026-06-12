#pragma once



// 여기엔 접근 가능한 것들 선언


bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize);
bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** spritefilePath, unsigned int spriteCount, const wchar_t** mp3filePath, unsigned int mp3Count, const wchar_t** mp4filePath, unsigned int mp4Count, const wchar_t** sfxfilePath, unsigned int sfxCount);


void DDANZIT_Run();


void DDANZIT_Finalize();
