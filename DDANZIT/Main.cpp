#include "DDANZIT.h"


int main()
{
	if (!DDANZIT_Initialize(L"Game", 1024, 760))
		return;

	DDANZIT_Run();

	DDANZIT_Finalize();
}
