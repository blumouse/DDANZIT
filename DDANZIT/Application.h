#pragma once


class Application
{
public:
	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend void DDANZIT_Run();
	friend void DDANZIT_Finalize();


private:
	static bool _isPlaying;
public:
	static bool& isPlaying() { return _isPlaying; }


private:
	static bool _isQuit;
public:
	static void Quit() { _isQuit = true; }

	// TODO: Quit / Pause µî.. Ãß°¡
};

