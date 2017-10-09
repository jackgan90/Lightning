#ifdef LIGHTNINGGE_WIN32
#include <Windows.h>	//for WinMain
#endif
#include "logger.h" //for logging
#include "memorysystem.h"
#include "appfactory.h" //for application

using LightningGE::App::AppFactory;
using LightningGE::Foundation::logger;
using LightningGE::Foundation::LogLevel;
#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	logger.Log(LogLevel::Info, "This is LightingGE entry.");
	auto pApp = AppFactory::Instance()->GetApp();
	if (!pApp->Init())
	{
		logger.Log(LogLevel::Error, "Application initialize error!");
		return 0;
	}
	if (!pApp->Start())
	{
		logger.Log(LogLevel::Error, "Application start error!");
		return 0;
	}
	int res = pApp->Run();

	pApp->Quit();
	logger.Log(LogLevel::Info, "Application quit.");
	AppFactory::Instance()->Finalize();
	return res;
}
