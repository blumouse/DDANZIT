#include "Debug.h"

#include "DefineOption.h"

#ifdef USE_DEBUG
#include <iostream>
#include <assert.h>
#include "INC_Windows.h"
#include "GameObject.h"

#ifdef USE_DEBUG_TUI
#include "SceneManager.h"
#include "Scene.h"
#include "Hierarchy.h"
#include "Transform.h"
#include <stack>

#endif // USE_DEBUG_TUI

#endif // USE_DEBUG

using namespace std;


void Debug::Log(const std::string& message)
{
#ifdef USE_DEBUG

	cout << message << endl;

#endif // USE_DEBUG

}


void Debug::Assert(bool condition) 
{
#ifdef USE_DEBUG

	assert(condition);

#endif // USE_DEBUG

}

void Debug::Assert(bool condition, const std::string& message)
{
#ifdef USE_DEBUG

	assert(condition && message.c_str());

#endif // USE_DEBUG

}


DebugConsole::DebugConsole()
{
	AllocConsole();

	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONIN$", "r", stdin);

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

	// 현재 보여지는 창의 가로(칸)와 세로(줄) 크기 계산
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

DebugConsole::~DebugConsole()
{
	fclose(stdout);
	fclose(stdin);

	FreeConsole();
}

// 콘솔 창을 보여줍니다.
void DebugConsole::Show()
{
	::ShowWindow(GetConsoleWindow(), SW_SHOW);
}

// 콘솔 창을 숨깁니다.
void DebugConsole::Hide()
{
	::ShowWindow(GetConsoleWindow(), SW_HIDE);
}

// 콘솔 창의 제목을 설정합니다.
void DebugConsole::SetTitle(const wchar_t* title)
{
	::SetConsoleTitle(title);
}


void DebugConsole::ToggleShow()
{
	HWND hwnd = GetConsoleWindow();
	if (hwnd)
	{
		if (::IsWindowVisible(hwnd))
		{
			Hide();
		}
		else
		{
			Show();
		}
	}
}

void DebugConsole::gotoxy(int x, int y)
{
	COORD pos = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void DebugConsole::hideCursor()
{
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.bVisible = 0; // false로 설정하여 커서 숨김
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}


ConsoleObject::ConsoleObject(int startX, int startY, int width, int height) : x(startX), y(startY), width(width), height(height)
{

}

void ConsoleObject::Draw()
{

}


Frame::Frame(int startX, int startY, int width, int height) : ConsoleObject(startX, startY, width, height)
{
}

void Frame::Draw()
{
	// TODO 대강 | 이런걸로 그려
}


Text::Text(int startX, int startY, int width, int height, std::string label) : ConsoleObject(startX, startY, width, height)
{
}

void Text::Draw()
{
}


Button::Button(int startX, int startY, int width, int height, string label) : Text(startX, startY, width, height, label)
{
	// "[ label ]" 형태이므로 길이는 글자수 + 4
	width = text.length() + 4;
}

bool Button::IsClicked(int clickX, int clickY)
{
	return (clickX >= x && clickX < x + width && clickY == y);
}

void Button::Draw()
{
	cout << text;
}


void Debug::InitializeDebugInfo()
{
	DebugConsole::hideCursor();

	// Run 직전..시작시에 최초 호출
	// 이때는 직접 발로 뛰면서 다 모든정보 수집하고 콘솔오브젝트 만들기
	for (Scene* scene : SceneManager::pLoadedSceneList)
	{
		ChangedHierarchyInfo(scene->hierarchy.pRootGameObjectList, scene);
	}
	
}


void Debug::ChangedSceneInfo()
{
}

void Debug::ChangedHierarchyInfo(std::vector<GameObject*>& gameObjectRoot, Scene* scene)
{
	string sceneName = scene->name();
	// TODO: 루트 개수가 0개면 씬 없어진걸로 대강 치고 지워버리기

	sceneHierarchyTree[sceneName].clear();
	sceneHierarchyTree[sceneName].push_back(Button(hierarchyOffsetX, hierarchyOffsetY, sceneName.size(), 1, sceneName));
	sceneHierarchyTree[sceneName].back().onClick = OnSceneButtonClick;


	int offsetY = hierarchyOffsetY + 1;

	for (GameObject* go : gameObjectRoot)
	{
		std::stack<std::pair<Transform*, int>> s;
		s.push({ go->_transform, 1 });

		while (!s.empty()) {
			Transform* curr = s.top().first;
			int depth = s.top().second;
			s.pop();


			string name = curr->_gameObject->_name;

			sceneHierarchyTree[sceneName].push_back(Button(hierarchyOffsetX + depth, offsetY, name.size(), 1, name));
			sceneHierarchyTree[sceneName].back().onClick = OnGameObjectButtonClick;
			offsetY++;


			for (int i = curr->childCount() - 1; i >= 0; --i) 
				s.push({ curr->pChildList[i], depth + 1 });
		}
	}
}

void Debug::UpdateDebugInfo()
{
}


void Debug::DrawDebugConsole()
{
}
