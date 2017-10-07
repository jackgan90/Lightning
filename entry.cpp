#ifdef LIGHTNINGGE_WIN32
#include <Windows.h>	//for WinMain
#endif
#include "logger.h" //for logging
#include "appfactory.h" //for application

using namespace LightningGE::App;
using LightningGE::Foundation::logger;
using LogLevel = LightningGE::Foundation::LogLevel;
#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	logger.Log(LightningGE::Foundation::Info, "This is LightingGE entry.");
	ApplicationPtr pApp = AppFactory::GetApp();
	if (!pApp->Init())
	{
		logger.Log(LightningGE::Foundation::Error, "Application initialize error!");
		return 0;
	}
	if (!pApp->Start())
	{
		logger.Log(LightningGE::Foundation::Error, "Application start error!");
		return 0;
	}
	int res = pApp->Run();

	pApp->Quit();
	logger.Log(LightningGE::Foundation::Info, "Application quit.");
	return res;
}
