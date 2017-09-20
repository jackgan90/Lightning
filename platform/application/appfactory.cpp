#include "appfactory.h"
#include <memory>
#ifdef LIGHTNINGGE_WIN32
#include "win32application.h"
#endif

namespace LightningGE
{
	namespace App
	{
		ApplicationPtr AppFactory::getApp()
		{
#ifdef LIGHTNINGGE_WIN32
			return std::make_shared<Win32Application>();
#endif
			return nullptr;
		}
	}
}