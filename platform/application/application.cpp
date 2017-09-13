#include "iapplication.h"
#ifdef LIGHTNINGGE_WIN32
#include "win32application.h"
#endif // LIGHTNINGGE_WIN32


namespace LightningGE
{
	namespace App
	{
		IApplication* IApplication::getApp()
		{
#ifdef LIGHTNINGGE_WIN32
			return new Win32Application();
#endif // LIGHTNINGGE_WIN32
			return nullptr;
		}
	}
}