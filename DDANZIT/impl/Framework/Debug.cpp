#include "Debug.h"

#ifdef USE_DEBUG
#include <iostream>
#include <format>
#include <assert.h>
#include "INC_Windows.h"
#include "GameObject.h"

#ifdef USE_DEBUG_TUI
#include <stack>
#include "SceneManager.h"
#include "Scene.h"
#include "Hierarchy.h"

#include "Transform.h"
#include "SpriteRenderer.h"
#include "Rigidbody2D.h"
#include "Collider2D.h"
#include "Camera.h"
#include "Text.h"

#include "Application.h"

#endif // USE_DEBUG_TUI

#endif // USE_DEBUG

using namespace std;


#pragma region DebugMethod

void Debug::Log(const std::string& message)
{
#ifdef USE_DEBUG

#ifdef USE_DEBUG_TUI
	if (console)
	{
		AddConsoleLog(message);
		return;
	}

#endif // USE_DEBUG_TUI

	std::cout << message << endl;

#endif // USE_DEBUG

}


void Debug::Assert(bool condition)
{
#ifdef USE_DEBUG

#ifdef USE_DEBUG_TUI
	if (console)
	{
		AddConsoleLog("Assert");
		Application::Pause();
		return;
	}


#endif // USE_DEBUG_TUI

	assert(condition);

#endif // USE_DEBUG

}

void Debug::Assert(bool condition, const std::string& message)
{
#ifdef USE_DEBUG

#ifdef USE_DEBUG_TUI
	if (console)
	{
		AddConsoleLog("Assert: " + message);
		Application::Pause();
		return;
	}

#endif // USE_DEBUG_TUI

	assert(condition && message.c_str());

#endif // USE_DEBUG

}

void Debug::Assert(bool condition, std::string message, GameObject* context)
{
#ifdef USE_DEBUG

#ifdef USE_DEBUG_TUI
	if (console)
	{
		AddConsoleLog("Assert: " + message + "\nName: " + context->_name + " " + format("\nAt: {}", static_cast<void*>(context)));
		Application::Pause();
		return;
	}

#endif // USE_DEBUG_TUI

	string assertion = message + "\nName: " + context->_name + " " + format("\nAt: {}", static_cast<void*>(context));
	assert(condition && assertion.c_str());

#endif // USE_DEBUG

}

#pragma endregion



#pragma region Console

HANDLE DebugConsole::hStdin = nullptr; 
HANDLE DebugConsole::hStdout = nullptr;


DebugConsole::DebugConsole()
{
	AllocConsole();

	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONIN$", "r", stdin);


	hStdin = CreateFile(TEXT("CONIN$"), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, 0, NULL);
	GetConsoleScreenBufferInfo(hStdin, &csbi);

	DWORD targetMode = ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;

	if (!SetConsoleMode(hStdin, targetMode))
		std::cout << "failed " << GetLastError();

	//COORD size = { 140, 40 };
	//SMALL_RECT rect = { 0, 0, 140, 50 };
	//SetConsoleScreenBufferSize(debugConsole.hStdin, size);
	//SetConsoleWindowInfo(debugConsole.hStdin, TRUE, &rect);
	std::cout << "\x1b[8;40;140t";

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	hideCursor();

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
	//SetConsoleCursorPosition(hStdin, pos);
	SetConsoleCursorPosition(hStdout, pos);
}

void DebugConsole::clearArea(int startX, int startY, int width, int height)
{
	// width 칸수만큼의 공백(Space)으로 꽉 찬 문자열을 한 번에 만듭니다.
	std::string emptyLine(width, ' ');

	// height 줄수만큼 아래로 내려가면서 공백 문자열을 덮어씌웁니다.
	for (int i = 0; i < height; ++i)
	{
		COORD pos = { (SHORT)startX, (SHORT)(startY + i) };
		SetConsoleCursorPosition(hStdout, pos);
		std::cout << emptyLine;
	}
}

void DebugConsole::hideCursor()
{
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.bVisible = 0; // false로 설정하여 커서 숨김
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(hStdout, &cursorInfo);
}

#pragma endregion


#ifdef USE_DEBUG_TUI

#pragma region ConsoleObject

ConsoleObject::ConsoleObject(int startX, int startY, int width, int height) : x(startX), y(startY), width(width), height(height), parent(nullptr)
{
}


ConsoleFrame::ConsoleFrame(int startX, int startY, int width, int height) : ConsoleObject(startX, startY, width, height), currentChildY(1)
{
}

void ConsoleFrame::AddChild(ConsoleObject* child)
{
	if (currentChildY == height - 1)
	{
		delete child;
		return;
	}

	child->SetParent(this);
	children.push_back(child);
	currentChildY++;
}

void ConsoleFrame::ClearChild()
{
	for (auto* child : children)
		delete child;

	children.clear();
	currentChildY = 1;
}

void ConsoleFrame::Draw()
{
	for (auto* child : children)
		child->Draw();


	DebugConsole::gotoxy(GetAbsoluteX(), GetAbsoluteY());

	// 상단
	cout << "+";
	for (int i = 1; i < width - 1; i++)
		cout << "-";
	cout << "+";

	for (int i = 1; i < height - 1; i++)
	{
		DebugConsole::gotoxy(GetAbsoluteX(), GetAbsoluteY() + i);
		cout << "|";

		DebugConsole::gotoxy(GetAbsoluteX() + width - 1, GetAbsoluteY() + i);
		cout << "|";
	}

	DebugConsole::gotoxy(GetAbsoluteX(), GetAbsoluteY() + height - 1);
	// 하단
	cout << "+";
	for (int i = 1; i < width - 1; i++)
		cout << "-";
	cout << "+";
}

bool ConsoleFrame::HandleClick(int clickX, int clickY)
{
	for (auto* child : children)
	{
		if (child->HandleClick(clickX, clickY))
			return true;
	}

	return false;
}


ConsoleInspector::ConsoleInspector(int startX, int startY, int width, int height) : ConsoleFrame(startX, startY, width, height)
{
}

void ConsoleInspector::AddChild(ConsolePollingText* child)
{
	if (currentChildY == height - 1)
	{
		delete child;
		return;
	}

	child->SetParent(this);
	childrenInspector.push_back(child);
	currentChildY++;
}

void ConsoleInspector::Draw()
{
	for (auto* child : childrenInspector)
		child->Draw();


	DebugConsole::gotoxy(GetAbsoluteX(), GetAbsoluteY());

	// 상단
	cout << "+";
	for (int i = 1; i < width - 1; i++)
		cout << "-";
	cout << "+";

	for (int i = 1; i < height - 1; i++)
	{
		DebugConsole::gotoxy(GetAbsoluteX(), GetAbsoluteY() + i);
		cout << "|";

		DebugConsole::gotoxy(GetAbsoluteX() + width - 1, GetAbsoluteY() + i);
		cout << "|";
	}

	DebugConsole::gotoxy(GetAbsoluteX(), GetAbsoluteY() + height - 1);
	// 하단
	cout << "+";
	for (int i = 1; i < width - 1; i++)
		cout << "-";
	cout << "+";
}

void ConsoleInspector::ClearChild()
{
	for (auto* child : childrenInspector)
		delete child;

	childrenInspector.clear();
	currentChildY = 1;
}


ConsoleText::ConsoleText(int startX, int startY, int width, int height, std::string t) : ConsoleObject(startX, startY, width, height), text(t)
{
}

void ConsoleText::Draw()
{
	DebugConsole::gotoxy(GetAbsoluteX(), GetAbsoluteY());
	cout << text;
}


ConsoleButton::ConsoleButton(int startX, int startY, int width, int height, string t, std::function<void()> callback) : ConsoleText(startX, startY, width, height, t), onClick(callback)
{
}

bool ConsoleButton::HandleClick(int clickX, int clickY)
{
	int absX = GetAbsoluteX();
	int absY = GetAbsoluteY();

	// 내 영역 안에서 클릭되었는지 확인
	if (clickX >= absX && clickX < absX + width && clickY == absY)
	{
		if (onClick)
			onClick();

		return true;
	}
	return false;
}


ConsolePollingText::ConsolePollingText(int x, int y, std::string t) :
	ConsoleText(x, y, 1, 1, t), valueGetter(nullptr)
{
}

ConsolePollingText::ConsolePollingText(int x, int y, std::function<std::string()> getter) :
	ConsoleText(x, y, 1, 1, ""), valueGetter(getter)
{
	lastValue = valueGetter();
	this->text = lastValue;
}

void ConsolePollingText::Update()
{
	if (valueGetter == nullptr)
		return;

	std::string currentValue = valueGetter();

	if (currentValue != lastValue)
	{
		lastValue = currentValue;
		this->text = currentValue;

		// 값이 바뀌었을 때만 화면의 해당 좌표로 가서 글자를 덮어씌움
		this->Draw();
	}
}

#pragma endregion



#pragma region DebugTUI

ConsoleFrame* Debug::console;

bool Debug::isManageChanged = false;
bool Debug::isConsoleChanged = false;
bool Debug::isSceneChanged = false;
bool Debug::isHierarchyChanged = false;


void Debug::InitializeDebugInfo()
{
	// 140, 40

	manage = new ConsoleFrame(0, 0, 50, 23);
	sceneList = new ConsoleFrame(50, 0, 24, 23);
	console = new ConsoleFrame(0, 23, 74, 16);

	hierarchy = new ConsoleFrame(74, 0, 24, 39);
	inspector = new ConsoleInspector(98, 0, 40, 39);

	// Run 직전..시작시에 최초 호출
	// 이때는 직접 발로 뛰면서 다 모든정보 수집하고 콘솔오브젝트 만들기
	DrawSceneList();
	BuildManage();
	DrawConsole();

	DrawHierarchy();
	inspector->Draw();
}

void Debug::FinalizeDebugInfo()
{
	inspector->ClearChild();
	hierarchy->ClearChild();

	console->ClearChild();
	manage->ClearChild();
	sceneList->ClearChild();

	delete inspector;
	delete hierarchy;

	delete console;
	delete manage;
	delete sceneList;
}


void Debug::ChangedSceneInfo()
{
	isSceneChanged = true;
}

void Debug::ChangedHierarchyInfo()
{
	isHierarchyChanged = true;
}


void Debug::AddConsoleLog(const std::string& message)
{
	// TODO: 이거 스택이 있어야되네
	console->AddChild(new ConsoleText(1, console->height - console->currentChildY - 1, message.size(), 1, message));
	isConsoleChanged = true;
}


void Debug::HandleDebugConsoleInput()
{
	// 비동기 처리: 이벤트가 있는지 갯수만 먼저 확인 (메인 루프가 멈추지 않음!)
	GetNumberOfConsoleInputEvents(debugConsole.hStdin, &numEvents);

	if (numEvents > 0) {
		ReadConsoleInput(debugConsole.hStdin, &ir, 1, &read);

		if (ir.EventType == MOUSE_EVENT)
		{
			// TODO
			if (ir.Event.MouseEvent.dwEventFlags == 0 && ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				COORD mousePos = ir.Event.MouseEvent.dwMousePosition;
				OnDebugConsoleClick(mousePos.X, mousePos.Y);
			}
		}
		else if (ir.EventType == KEY_EVENT)
		{
			// 키를 '누를 때'만 반응 (뗄 때는 무시)
			if (ir.Event.KeyEvent.bKeyDown)
			{
				char key = ir.Event.KeyEvent.uChar.AsciiChar;

				if (key == '\r')
				{
					// [엔터키] 명령어 실행 후 입력창 초기화
					ExecuteCommand(commandLine);
					LogCommand(commandLine);
					commandLine = "";
				}
				else if (key == '\b') {
					// [백스페이스키] 글자 지우기
					if (!commandLine.empty())
					{
						commandLine.pop_back();
					}
				}
				else if (key >= 32 && key <= 126)
				{
					// [일반 문자] 스페이스바(32)부터 물결표(126)까지의 출력 가능한 문자만 추가
					commandLine += key;
				}
			}
		}

		DebugConsole::gotoxy(2, 20);
		cout << "Cmd > " << commandLine << "               ";
	}
}

void Debug::ExecuteCommand(std::string cmd)
{
	// TODO: 파싱...
	// 근데 무슨 명령어 쓰지
}


void Debug::OnDebugConsoleClick(int x, int y)
{
	if (manage->HandleClick(x, y))
		return;
	if (hierarchy->HandleClick(x, y))
		return;

	// 외에는 버튼이 없어서 볼것도 없다

	highlightedObject = nullptr;
}


void Debug::BuildManage()
{
	// 56 24, 0~55 / 0~23

	// TODO
	// 대강 명령 버튼들 / 명령어들
	// timescale pause consoleclear
	// 19 20번 줄은 커맨드 입력 / 백로그 칸

	manage->AddChild(new ConsoleButton(30, 1, 14, 1, "[Pause/Resume]", [&]() {
		if (!Application::_isPause)
			Application::Pause();
		else
			Application::Resume();
		}));
	manage->AddChild(new ConsoleButton(1, 21, 7, 1, "[Clear]", [&]() {
		ClearConsole();
		}));
	manage->AddChild(new ConsoleText(4, 17, 11, 1, "=== CMD ==="));

	isManageChanged = true;
}

void Debug::DrawManage()
{
	DebugConsole::clearArea(manage->x + 1, manage->y + 1, manage->width - 2, manage->height - 2);
	manage->Draw();
}

void Debug::LogCommand(const std::string& message)
{
	if (log == nullptr)
	{
		log = new ConsoleText(1, 19, message.size(), 1, message);
		manage->AddChild(log);
	}
	else
	{
		log->text = message;
	}

	isManageChanged = true;
}


void Debug::DrawConsole()
{
	DebugConsole::clearArea(console->x + 1, console->y + 1, console->width - 2, console->height - 2);
	console->Draw();
}

void Debug::ClearConsole()
{
	console->ClearChild();
	DebugConsole::clearArea(console->x + 1, console->y + 1, console->width - 2, console->height - 2);
}


void Debug::DrawSceneList()
{
	// 이 시점에서 정보 찾고 콘솔오브젝트 갱신 후 바로 그리기
	sceneList->ClearChild();
	DebugConsole::clearArea(sceneList->x + 1, sceneList->y + 1, sceneList->width - 2, sceneList->height - 2);

	for (Scene* scene : SceneManager::pSceneList)
	{
		string name = scene->name().substr(0, sceneList->width - 4);

		sceneList->AddChild(new ConsoleText(1, sceneList->currentChildY, (name.size() + 2), 1, "- " + name));
	}

	for (Scene* scene : SceneManager::pSceneInstanceList)
	{
		string name = scene->name().substr(0, sceneList->width - 4);

		sceneList->AddChild(new ConsoleText(1, sceneList->currentChildY, (name.size() + 2), 1, "- " + name));
	}


	sceneList->Draw();
}

void Debug::DrawHierarchy()
{
	// 이 시점에서 정보 찾고 콘솔오브젝트 갱신 후 바로 그리기
	hierarchy->ClearChild();
	DebugConsole::clearArea(hierarchy->x + 1, hierarchy->y + 1, hierarchy->width - 2, hierarchy->height - 2);

	for (Scene* scene : SceneManager::pLoadedSceneList)
	{
		string name = " = " + scene->name().substr(0, hierarchy->width - 5);

		hierarchy->AddChild(new ConsoleText(1, hierarchy->currentChildY, name.size(), 1, name));


		for (GameObject* go : scene->GetRootGameObjects())
		{
			std::stack<std::pair<Transform*, int>> s;
			s.push({ go->_transform, 2 });

			while (!s.empty()) {
				Transform* curr = s.top().first;
				int depth = s.top().second;
				s.pop();


				string name = curr->_gameObject->_name.substr(0, hierarchy->width - 2 - depth);

				hierarchy->AddChild(new ConsoleButton(depth, hierarchy->currentChildY, name.size(), 1, name, [=]() {
					highlightedObject = curr->_gameObject;
					BuildInspector();
					}));


				for (int i = curr->childCount() - 1; i >= 0; --i)
					s.push({ curr->pChildList[i], depth + 1 });
			}
		}
	}

	hierarchy->Draw();
}

void Debug::BuildInspector()
{
	inspector->ClearChild();
	DebugConsole::clearArea(inspector->x + 1, inspector->y + 1, inspector->width - 2, inspector->height - 2);

	// 추적하고 있다가 오브젝트 날라가면 어카지?
	// 이거 draw타이밍이 렌더 타이밍이니까.. iskilled 체크할수 있겠네
	if (highlightedObject)
	{
		if (highlightedObject->isKilled)
		{
			highlightedObject = nullptr;
			inspector->ClearChild();
			inspector->Draw();
			return;
		}

		string active;
		string name;


		// 먼저 오브젝트 정보 액티브 이름
		if (!highlightedObject->_active || !highlightedObject->parentActive)
			name = " == ○ " + highlightedObject->_name.substr(0, inspector->width - 4) + " ==";
		else
			name = " == ● " + highlightedObject->_name.substr(0, inspector->width - 4) + " ==";

		inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, name));
		inspector->currentChildY++;		// 한줄 띄우기

		// 트랜스폼 따로
		Transform* tr = highlightedObject->_transform;

		inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, "=   Transform ="));

		// 넓이계산 포기; 차피 버튼 아님
		inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [tr]() {
			return format("Position\t    X {}  Y {}      ", tr->_localPosition.x, tr->_localPosition.y);
			}));
		inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [tr]() {
			return format("Angle\t    {}    ", tr->localAngle());
			}));
		inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [tr]() {
			return format("Scale\t    X {}  Y {}      ", tr->_localScale.x, tr->_localScale.y);
			}));
		inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [tr]() {
			return format("Depth\t    {}    ", tr->_depth);
			}));


		for (Component* comp : highlightedObject->pComponentList)
		{
			string_view type = comp->componentTypeName;

			name = type;

			if (!comp->_active || !comp->parentActive)
				active = "= ○ ";
			else
				active = "= ● ";

			inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, active + name + " ="));

			if (type == "MonoBehavior")
			{
				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, "(Script)"));
			}
			else if (type == "SpriteRenderer")
			{
				SpriteRenderer* sp = static_cast<SpriteRenderer*>(comp);

				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [sp]() {
					return format("SpriteIndex\t    {}    ", (int)sp->sprite);
					}));
				if (sp->useAtlas)
				{
					inspector->AddChild(new ConsolePollingText(2, inspector->currentChildY, [sp]() {
						return format("Atlas\t    X:{} Y:{} W:{} H:{}      ", sp->currentAtlas.pixel_OffsetX, sp->currentAtlas.pixel_OffsetY, sp->currentAtlas.pixel_Width, sp->currentAtlas.pixel_Height);
						}));
				}
				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [sp]() {
					return format("Color\t    R:{} G:{} B:{} A:{}      ", sp->color.r, sp->color.g, sp->color.b, sp->color.a);
					}));
				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [sp]() {
					return format("Flip\t\t    X:{}  Y:{}    ", sp->flipX ? 'o' : 'x', sp->flipY ? 'o' : 'x');
					}));
			}
			else if (type == "Rigidbody2D")
			{
				Rigidbody2D* rigid = static_cast<Rigidbody2D*>(comp);

				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [rigid]() {
					return format("Body Type\t    {}    ", rigid->bodyType == RigidBodyType2D::Kinematic ? "Kinematic" : (rigid->bodyType == RigidBodyType2D::Static ? "Static" : "Dynamic"));
					}));
			}
			else if (type == "BoxCollider2D")
			{
				BoxCollider2D* col = static_cast<BoxCollider2D*>(comp);

				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [col]() {
					return format("Is Trigger\t\t    {}  ", col->_isTrigger ? 'o' : 'x');
					}));
				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [col]() {
					return format("Offset\t    X:{}  Y:{}    ", col->_offset.x, col->_offset.y);
					}));
				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [col]() {
					return format("Size\t\t    X:{}  Y:{}    ", col->_size.x, col->_size.y);
					}));
			}
			else if (type == "CircleCollider2D")
			{
				CircleCollider2D* col = static_cast<CircleCollider2D*>(comp);

				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [col]() {
					return format("Is Trigger\t    {}  ", col->_isTrigger ? 'o' : 'x');
					}));
				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [col]() {
					return format("Offset\t    X:{}  Y:{}    ", col->_offset.x, col->_offset.y);
					}));
				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [col]() {
					return format("Radius\t    {}    ", col->_size.x);
					}));
			}
			else if (type == "Camera")
			{
				Camera* cam = static_cast<Camera*>(comp);

				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [tr]() {
					return format("Depth\t    {}    ", tr->_depth);
					}));
			}
			else if (type == "Text")
			{
				Text* t = static_cast<Text*>(comp);

				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [t]() {
					return format("Text\t\t    {}", "wchar라 곤란");
					}));
				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [t]() {
					return format("Font(Index)\t    {}    ", (int)t->font);
					}));
				
				int size = 0;
				switch (t->fontSize)
				{
				default:
					break;
				case FontSize::_12:
					size = 12;	break;
				case FontSize::_14:
					size = 14;	break;
				case FontSize::_16:
					size = 16;	break;
				case FontSize::_18:
					size = 18;	break;
				case FontSize::_20:
					size = 20;	break;
				case FontSize::_24:
					size = 24;	break;
				case FontSize::_28:
					size = 28;	break;
				case FontSize::_32:
					size = 32;	break;
				case FontSize::_36:
					size = 36;	break;
				case FontSize::_40:
					size = 40;	break;
				case FontSize::_48:
					size = 48;	break;
				case FontSize::_60:
					size = 60;	break;
				}

				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [t, size]() {
					return format("FontSize\t    {}  ", size);
					}));
				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [t]() {
					return format("Color\t    R:{} G:{} B:{} A:{}      ", t->color.r, t->color.g, t->color.b, t->color.a);
					}));
				inspector->AddChild(new ConsolePollingText(1, inspector->currentChildY, [t]() {
					return format("Flip\t\t    X:{}  Y:{}  ", t->flipX ? 'o' : 'x', t->flipY ? 'o' : 'x');
					}));
			}
		}
	}

	inspector->Draw();
}

void Debug::UpdateInspector()
{
	if (highlightedObject)
	{
		if (highlightedObject->isKilled)
		{
			highlightedObject = nullptr;
			inspector->ClearChild();
			inspector->Draw();
			return;
		}

		for (ConsolePollingText* child : inspector->childrenInspector)
			child->Update();
	}
}


void Debug::DrawDebugConsole()
{
	UpdateInspector();

	if (isManageChanged)
	{
		DrawManage();
		isManageChanged = false;
	}

	if (isConsoleChanged)
	{
		DrawConsole();
		isConsoleChanged = false;
	}

	if (isHierarchyChanged)
	{
		DrawHierarchy();
		isHierarchyChanged = false;
	}

	if (isSceneChanged)
	{
		DrawSceneList();
		isSceneChanged = false;
	}

	DebugConsole::gotoxy(139, 39);
}

#pragma endregion

#endif // USE_DEBUG_TUI
