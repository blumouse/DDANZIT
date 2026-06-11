#pragma once


class Application
{
public:
	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize);
	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** spritefilePath, unsigned int spriteCount, const wchar_t** mp3filePath, unsigned int mp3Count, const wchar_t** mp4filePath, unsigned int mp4Count, const wchar_t** sfxfilePath, unsigned int sfxCount);
	friend void DDANZIT_Run();
	friend void DDANZIT_Finalize();


private:
	static bool _isPlaying;
public:
	static bool isPlaying() { return _isPlaying; }


private:
	static bool _isQuit;
public:
	static void Quit() { _isQuit = true; }

	// TODO: ¥Ÿ∏•∞≈ ∂« ππ¿’¡ˆ
};

