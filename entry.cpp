#ifdef LIGHTNINGGE_WIN32
#include <Windows.h>	//for WinMain
#include "win32application.h"
#endif
#include "logger.h" //for logging
#include "memorysystem.h"

using LightningGE::Foundation::logger;
using LightningGE::Foundation::LogLevel;
#ifdef LIGHTNINGGE_WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	logger.Log(LogLevel::Info, "This is LightingGE entry.");
#ifdef LIGHTNINGGE_WIN32
	LightningGE::App::Win32Application theApp;
#endif
	theApp.Start();
	logger.Log(LogLevel::Info, "Application start successfully!");
	return theApp.Run();
}
