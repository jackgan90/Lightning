#ifdef LIGHTNING_WIN32
#include <Windows.h>	//for WinMain
#include "win32application.h"
#endif
#include "logger.h" //for logging

using Lightning::Foundation::logger;
using Lightning::Foundation::LogLevel;
#ifdef LIGHTNING_WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	logger.Log(LogLevel::Info, "This is LightingGE entry.");
#ifdef LIGHTNING_WIN32
	Lightning::App::Win32Application theApp;
#endif
	theApp.Start();
	logger.Log(LogLevel::Info, "Application start successfully!");
	return theApp.Run();
}
