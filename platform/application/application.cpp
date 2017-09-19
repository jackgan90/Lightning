#include "iapplication.h"
#ifdef LIGHTNINGGE_WIN32
#include "win32application.h"
#endif // LIGHTNINGGE_WIN32


namespace LightningGE
{
	namespace App
	{
		ApplicationPtr IApplication::getApp()
		{
#ifdef LIGHTNINGGE_WIN32
			return std::make_shared<Win32Application>();
#endif // LIGHTNINGGE_WIN32
			return nullptr;
		}
	}
}