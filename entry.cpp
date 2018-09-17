#ifdef LIGHTNING_WIN32
#include <Windows.h>	//for WinMain
#include "win32application.h"
#endif
#include "logger.h" //for logging

#ifdef LIGHTNING_WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	LOG_INFO("This is LightingGE entry.");
#ifdef LIGHTNING_WIN32
	Lightning::App::Win32Application theApp;
#endif
	theApp.Start();
	LOG_INFO("Application start successfully!");
	return theApp.Run();
}
