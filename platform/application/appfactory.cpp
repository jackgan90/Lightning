#include <memory>
#include "appfactory.h"

namespace LightningGE
{
	namespace App
	{

#ifdef LIGHTNINGGE_WIN32
		ApplicationPtr<Win32Application::allocator_type> AppFactory::s_app = nullptr;

		ApplicationPtr<Win32Application::allocator_type> AppFactory::GetApp()
		{
			if (!s_app)
			{
				s_app = ApplicationPtr<Win32Application::allocator_type>(new Win32Application());
			}
			return s_app;
		}
#else
		//basically this branch should never execute
		ApplicationPtr<Application, Application::allocator_type> AppFactory::s_app = nullptr;

		ApplicationPtr<Application, Application::allocator_type> AppFactory::GetApp()
		{
			return nullptr;
		}

#endif

		void AppFactory::Finalize()
		{
			s_app.reset();
		}
	}
}