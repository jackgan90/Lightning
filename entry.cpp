#include "logging\logger.h" //for logging
#include "platform\application\iapplication.h" //for application
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
	IApplication* pApp = IApplication::getApp();
	if (!pApp->Init())
		return 1;
	if (!pApp->Start())
		return 1;
	if (pApp->Run())
		return 1;
	pApp->Quit();
	delete pApp;
	return 0;
}
