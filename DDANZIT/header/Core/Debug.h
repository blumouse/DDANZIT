#pragma once

#include <string>

// 콘솔 창 자체도 이 근처에서 켜주는게 좋을듯

class Debug
{

public:
	static void Log(std::string message);

	static void Assert(bool condition);
	static void Assert(bool condition, std::string message);
	//static void Assert(bool condition, std::string message, Object context);

	// static void Break(); 일단 Application Pause랑 연동

};

