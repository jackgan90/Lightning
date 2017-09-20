#include "logger.h" //for logging
#include "appfactory.h" //for application
#ifdef LIGHTNINGGE_WIN32
#include <Windows.h>	//for WinMain
#endif

using namespace LightningGE::App;
using LightningGE::Utility::logger;
using LogLevel = LightningGE::Utility::LogLevel;
#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	logger.Log(LightningGE::Utility::Info, "This is LightingGE entry.");
	ApplicationPtr pApp = AppFactory::getApp();
	if (!pApp->Init())
	{
		logger.Log(LightningGE::Utility::Error, "Application initialize error!");
		return 1;
	}
	if (!pApp->Start())
	{
		logger.Log(LightningGE::Utility::Error, "Application start error!");
		return 1;
	}
	if (pApp->Run())
	{
		logger.Log(LightningGE::Utility::Error, "Application run error!");
		return 1;
	}
	pApp->Quit();
	logger.Log(LightningGE::Utility::Info, "Application quit.");
	return 0;
}
