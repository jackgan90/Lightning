#include "platform\iwindow.h"
#include "platform\windowmanager.h"
#include "logger.h"
#ifdef LIGHTNINGGE_WIN32
#include <Windows.h>
#endif


using namespace LightningGE::WindowSystem;
#ifdef _MSC_VER
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	LightningGE::Utility::logger.Log(LightningGE::Utility::Info, "This is LightingGE entry.");
	auto pWindow = WindowManager::Instance()->MakeWindow();
	pWindow->Init();
	pWindow->Show(true);
	//system("pause");
	return 0;
}
