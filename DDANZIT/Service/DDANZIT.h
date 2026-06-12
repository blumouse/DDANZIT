#pragma once


enum class ResourceType
{
	Sprite,
	Media,
	Font,
};


// 여기엔 접근 가능한 것들 선언


bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);

bool DDANZIT_LoadResources(ResourceType type, const wchar_t** pFilePath, unsigned int size);
bool DDANZIT_LoadResources(ResourceType type, 
	const wchar_t** mp3filePath, unsigned int mp3Count,
	const wchar_t** mp4filePath, unsigned int mp4Count,
	const wchar_t** sfxfilePath, unsigned int sfxCount);

void DDANZIT_Run();


void DDANZIT_Finalize();
