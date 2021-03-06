#include "ApplicationFactory.h"
#ifdef LIGHTNING_WIN32
#include "WindowsApplication.h"
#endif

namespace Lightning
{
	namespace App
	{
		std::unique_ptr<IApplication> ApplicationFactory::CreateApplication()
		{
#ifdef LIGHTNING_WIN32
			return std::make_unique<WindowsApplication>();
#endif
			return nullptr;
		}
	}
}