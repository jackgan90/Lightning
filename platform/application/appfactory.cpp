#include <memory>
#include "appfactory.h"
#ifdef LIGHTNINGGE_WIN32
#include "win32application.h"
#endif

namespace LightningGE
{
	namespace App
	{
		ApplicationPtr AppFactory::s_app = nullptr;
		ApplicationPtr AppFactory::GetApp()
		{
			if (!s_app)
			{
			#ifdef LIGHTNINGGE_WIN32
				s_app = ApplicationPtr(new Win32Application());
			#endif
			}
			return s_app;
		}
		void AppFactory::Finalize()
		{
			s_app.reset();
		}
	}
}