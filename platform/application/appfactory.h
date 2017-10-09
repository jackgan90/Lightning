#pragma once
#include "platformexportdef.h"
#include "iapplication.h"
#include "singleton.h"
#ifdef LIGHTNINGGE_WIN32
#include "win32application.h"
#endif

namespace LightningGE
{
	namespace App
	{
		class LIGHTNINGGE_PLATFORM_API AppFactory : public Foundation::Singleton<AppFactory>
		{
		public:
			void Finalize();
#ifdef LIGHTNINGGE_WIN32
		public:
			IApplication<Win32Application>* GetApp();
#else
		public:
			IApplication<Application>* GetApp();
#endif
		};

	}
}