#include "Debug.h"

#include "DefineOption.h"

#ifdef USE_DEBUG

	#include <iostream>
	#include <assert.h>

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