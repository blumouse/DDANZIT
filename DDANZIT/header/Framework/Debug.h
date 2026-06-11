#pragma once

#include "INC_Windows.h"
#include <string>
#include <list>
#include <unordered_map>
#include <vector>
#include <functional>

#include "DefineOption.h"

class GameObject;
class Scene;
class Debug;


class DebugConsole
{
    DebugConsole(const DebugConsole&) = delete;
    DebugConsole& operator=(const DebugConsole&) = delete;
    DebugConsole(DebugConsole&&) = delete;
    DebugConsole& operator=(DebugConsole&&) = delete;
public:
    DebugConsole();
    ~DebugConsole();

    HANDLE hStdin;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns, rows;

    static void Show();
    static void Hide();
    static void SetTitle(const wchar_t* title);
    static void ToggleShow();

    static void gotoxy(int x, int y);
    static void hideCursor();
};


#ifdef USE_DEBUG_TUI

class ConsoleObject
{
public:
    int x, y, width, height;
    ConsoleObject* parent;

    ConsoleObject(int startX, int startY, int width, int height);
    virtual ~ConsoleObject() = default;

    void SetParent(ConsoleObject* p) { parent = p; }

    int GetAbsoluteX() { return parent ? parent->GetAbsoluteX() + x : x; }
    int GetAbsoluteY() { return parent ? parent->GetAbsoluteY() + y : y; }

    virtual void Draw() = 0;
    virtual bool HandleClick(int x, int y) { return false; }
};

class ConsoleFrame : public ConsoleObject
{
public:
    std::vector<ConsoleObject*> children;
    int currentChildY;

    ConsoleFrame(int startX, int startY, int width, int height);

    void AddChild(ConsoleObject* child);
    void ClearChild();

    void Draw() override;
    bool HandleClick(int clickX, int clickY) override;
};

class ConsoleText : public ConsoleObject
{
public:
    std::string text;

    ConsoleText(int startX, int startY, int width, int height, std::string t);

    void Draw() override;
};

class ConsoleButton : public ConsoleText
{
public:
    std::function<void()> onClick;

    ConsoleButton(int startX, int startY, int width, int height, std::string t, std::function<void()> callback);

    bool HandleClick(int clickX, int clickY) override;
};

class ConsolePollingText : public ConsoleText
{
public:
    std::function<std::string()> valueGetter; // 값을 가져오는 함수
    std::string lastValue;                    // 이전 프레임의 캐싱된 값

    ConsolePollingText(int x, int y, std::string t);
    ConsolePollingText(int x, int y, std::function<std::string()> getter);

    void Update();
};

class ConsoleInspector : public ConsoleFrame
{
public:
    std::vector<ConsolePollingText*> childrenInspector;

    ConsoleInspector(int startX, int startY, int width, int height);

    void AddChild(ConsolePollingText* child);
    void ClearChild();
};


#endif // USE_DEBUG_TUI



class Debug
{
public:
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
    friend void DDANZIT_Run();
    friend void DDANZIT_Finalize();

public:
    Debug() = default;
private:
    Debug(const Debug&) = delete;
    Debug& operator=(const Debug&) = delete;
    Debug(Debug&&) = delete;
    Debug& operator=(Debug&&) = delete;


    // --- TUI ---
#ifdef USE_DEBUG_TUI

private:
    DebugConsole debugConsole;

    ConsoleFrame* manage;
    static ConsoleFrame* console;      // ?

    ConsoleFrame* sceneList;
    ConsoleFrame* hierarchy;
    ConsoleInspector* inspector;

    static bool isManageChanged;
    static bool isConsoleChanged;
    static bool isSceneChanged;
    static bool isHierarchyChanged;
public:
    GameObject* highlightedObject;

    // 씬 리스트                                        -> 변할때만 전파받아서 바꾸기 (씬매니저에서)
    // 하이라키 (씬 + 오브젝트 트리)                    -> 이것도 변할때만 전파받기   (하이라키에서)
    // 인스펙터 (컴포넌트 이름정도만 인젝션해놓을까)    -> 현재 띄워진 오브젝트 지정해서 그 값만 계속 감시
    // 로그 콘솔                                        -> 뭐 똑같지만.. 전파받기지 뭐
    // 간단한 인풋 커맨드? (pause stop TimeScale .. 버튼으로 해도되고)  -> 몰라

    void InitializeDebugInfo();
    void FinalizeDebugInfo();

    static void ChangedSceneInfo();        // 이건 직접 가서..주소니까 씬리스트 이름보고오는게?
    static void ChangedHierarchyInfo();    // 이것도 뭐 사실 직접 이름/자식 봐야지

    static void AddConsoleLog(const std::string& message);


public:
    void HandleDebugConsoleInput();

private:
    void OnDebugConsoleClick(int x, int y);
    void ExecuteCommand(std::string cmd);

    void BuildManage();
    void DrawManage();
    void LogCommand(const std::string& message);
    ConsoleText* log = nullptr;
    std::string commandLine = "";
    INPUT_RECORD ir;
    DWORD read;
    DWORD numEvents = 0;

    void DrawConsole();
    void ClearConsole();

    void DrawSceneList();

    void DrawHierarchy();

    void BuildInspector();
    void UpdateInspector();


public:
    void DrawDebugConsole();             // 전체 그리기


#endif // USE_DEBUG_TUI


public:
	static void Log(const std::string& message);

	static void Assert(bool condition);
	static void Assert(bool condition, const std::string& message);
	static void Assert(bool condition, std::string message, GameObject* context);

	// static void Break(); 일단 Application Pause랑 연동
};
