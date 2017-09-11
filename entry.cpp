#include <iostream>
#include <cstdlib>
#include "platform\iwindow.h"
#include "platform\windowmanager.h"
#ifdef LIGHTNINGGE_WIN32
#include <Windows.h>
#endif


using namespace std;
using namespace LightningGE::WindowSystem;
#ifdef _MSC_VER
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	cout << "This is LightingGE entry." << endl;
	auto pWindow = WindowManager::Instance()->MakeWindow();
	pWindow->Init();
	pWindow->Show(true);
	//system("pause");
	return 0;
}
