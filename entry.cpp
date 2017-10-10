#ifdef LIGHTNINGGE_WIN32
#include <Windows.h>	//for WinMain
#include "win32application.h"
#endif
#include "logger.h" //for logging
#include "memorysystem.h"

using LightningGE::Foundation::logger;
using LightningGE::Foundation::LogLevel;
#ifdef LIGHTNINGGE_WIN32
LightningGE::App::Win32Application* pApp = nullptr;
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	logger.Log(LogLevel::Info, "This is LightingGE entry.");
#ifdef LIGHTNINGGE_WIN32
	LightningGE::App::Win32Application theApp;
	pApp = &theApp;
#endif
	if (!theApp.Init())
	{
		logger.Log(LogLevel::Error, "Application initialize error!");
		return 0;
	}
	theApp.Start();
	int res = theApp.Run();

	logger.Log(LogLevel::Info, "Application quit.");
	return res;
}
