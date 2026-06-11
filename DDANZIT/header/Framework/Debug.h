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
    friend class SceneManager;
    friend class Hierarchy;
    friend class Transform;

    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height, const wchar_t** pfilePath, unsigned int resourceSize);
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


private:
    void InitializeDebugInfo();
    void FinalizeDebugInfo();

    static void ChangedSceneInfo();        // 이건 직접 가서..주소니까 씬리스트 이름보고오는게?
    static void ChangedHierarchyInfo();    // 이것도 뭐 사실 직접 이름/자식 봐야지

    static void AddConsoleLog(const std::string& message);


    void HandleDebugConsoleInput();

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


    void DrawDebugConsole();             // 전체 그리기


#endif // USE_DEBUG_TUI


public:
	static void Log(const std::string& message);

	static void Assert(bool condition);
	static void Assert(bool condition, const std::string& message);
	static void Assert(bool condition, std::string message, GameObject* context);

	// static void Break(); 일단 Application Pause랑 연동
};
