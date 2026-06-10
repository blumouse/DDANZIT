#pragma once

#include "INC_Windows.h"
#include <string>
#include <list>
#include <unordered_map>
#include <vector>

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

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns, rows;

    static void Show();
    static void Hide();
    static void SetTitle(const wchar_t* title);
    static void ToggleShow();

    static void gotoxy(int x, int y);
    static void hideCursor();

};


class ConsoleObject
{
public:
    int x, y, width, height;

    ConsoleObject(int startX, int startY, int width, int height);
    virtual ~ConsoleObject() = default;

    // 자기 자신을 콘솔에 그리는 함수
    virtual void Draw();
};

class ConsoleFrame : public ConsoleObject
{
public:
    ConsoleFrame(int startX, int startY, int width, int height);

    void Draw() override;
};

class ConsoleText : public ConsoleObject
{
public:
    std::string text;

    ConsoleText(int startX, int startY, int width, int height, std::string label);

    void Draw() override;
};

class ConsoleButton : public ConsoleText
{
public:
    //임시
    void (Debug::*onClick)() = nullptr;

    ConsoleButton(int startX, int startY, int width, int height, std::string label);

    // 마우스 좌표가 들어왔을 때 자기 영역인지 확인하는 함수
    bool IsClicked(int clickX, int clickY);
    void Draw() override;
};

class Debug
{
public:
    friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
    friend void DDANZIT_Run();
    friend void DDANZIT_Finalize();

    Debug(const Debug&) = delete;
    Debug& operator=(const Debug&) = delete;
    Debug(Debug&&) = delete;
    Debug& operator=(Debug&&) = delete;

private:
    DebugConsole console;




    bool isSceneChanged;
    std::vector<std::pair<Scene*, ConsoleText>> sceneList;
    //std::unordered_map<Scene*, ConsoleText> loadedSceneList;
    std::vector<std::pair<Scene*, ConsoleText>> loadedSceneList;

    bool isHierarchyChanged;
    const int hierarchyOffsetX = 60;
    const int hierarchyOffsetY = 3;
    std::unordered_map<Scene*, std::vector<std::pair<GameObject*, ConsoleButton>>> sceneHierarchyTree;

    GameObject* highlightedObject;
    std::vector<std::pair<GameObject*, ConsoleText>> componentList;

    // 씬 리스트                                        -> 변할때만 전파받아서 바꾸기 (씬매니저에서)
    // 하이라키 (씬 + 오브젝트 트리)                    -> 이것도 변할때만 전파받기   (하이라키에서)
    // 인스펙터 (컴포넌트 이름정도만 인젝션해놓을까)    -> 현재 띄워진 오브젝트 지정해서 그 값만 계속 감시
    // 로그 콘솔                                        -> 뭐 똑같지만.. 전파받기지 뭐
    // 간단한 인풋 커맨드? (pause stop TimeScale .. 버튼으로 해도되고)  -> 몰라

    void InitializeDebugInfo();

    void ChangedSceneInfo();        // 이건 직접 가서..주소니까 씬리스트 이름보고오는게?
    void ChangedHierarchyInfo();    // 이것도 뭐 사실 직접 이름/자식 봐야지
    void UpdateDebugInfo();         // 저 오브젝트 컴포넌트값들 매 프레임 감시
    // 클릭된건 어느타이밍에 아는거지?

    // 등등
    void DrawSceneList();
    void DrawHierarchy();
    void DrawInspector();
    void DrawConsole();

    void DrawDebugConsole();             // 바뀐것만 다시 그려! 정보는 여기 다 있으니


    void OnSceneButtonClick()
    {

    }

    void OnGameObjectButtonClick()
    {

    }



public:
	static void Log(const std::string& message);

	static void Assert(bool condition);
	static void Assert(bool condition, const std::string& message);
	//static void Assert(bool condition, std::string message, Object context);

	// static void Break(); 일단 Application Pause랑 연동
};
