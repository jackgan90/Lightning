#include <memory>
#include "appfactory.h"

namespace LightningGE
{
	namespace App
	{

#ifdef LIGHTNINGGE_WIN32
		IApplication<Win32Application>* AppFactory::GetApp()
		{
			return Win32Application::Instance();
		}
#else
		//basically this branch should never execute
		IApplication<Application> AppFactory::GetApp()
		{
			return nullptr;
		}

#endif
		void AppFactory::Finalize()
		{
		}
	}
}