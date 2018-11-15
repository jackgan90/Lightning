#include "ApplicationFactory.h"
#ifdef LIGHTNING_WIN32
#include "WindowsApplication.h"
#endif

namespace Lightning
{
	namespace App
	{
		Application* ApplicationFactory::CreateApplication()
		{
#ifdef LIGHTNING_WIN32
			return new WindowsApplication();
#endif
			return nullptr;
		}
	}
}