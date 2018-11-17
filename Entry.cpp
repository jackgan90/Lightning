#ifdef LIGHTNING_WIN32
#include <Windows.h>	//for WinMain
#endif
#include "Logger.h" //for logging
#include "Engine.h"

#ifdef LIGHTNING_WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	return Lightning::Engine::Instance()->Run();
}
