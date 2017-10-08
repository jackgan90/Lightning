#pragma once
#include "platformexportdef.h"
#include "iapplication.h"
#ifdef LIGHTNINGGE_WIN32
#include "win32application.h"
#endif

namespace LightningGE
{
	namespace App
	{
		class LIGHTNINGGE_PLATFORM_API AppFactory
		{
		public:
			static void Finalize();
#ifdef LIGHTNINGGE_WIN32
		public:
			static ApplicationPtr<Win32Application::allocator_type> GetApp();
		private:
			static ApplicationPtr<Win32Application::allocator_type> s_app;
#else
		public:
			static ApplicationPtr<Application::allocator_type> GetApp();
		private:
			static ApplicationPtr<Application::allocator_type> s_app;
#endif
		};

	}
}