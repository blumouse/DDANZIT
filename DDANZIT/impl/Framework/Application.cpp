#include "Application.h"

#include "GameTime.h"


bool Application::_isPlaying = false;

bool Application::_isQuit = false;

bool Application::_isPause = false;


void Application::Pause() 
{ 
	_isPause = true;
	Time::Pause();
}

void Application::Resume() 
{ 
	_isPause = false; 
	Time::Resume();
}
