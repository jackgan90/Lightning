#pragma once
#include "platformexportdef.h"
#include "iapplication.h"
#include "iwindow.h"
#include "windowmanager.h"
#include "filesystem.h"
#include "memorysystem.h"

namespace Lightning
{
	namespace App
	{
		using WindowSystem::SharedWindowPtr;
		using WindowSystem::WindowManager;
		class LIGHTNING_PLATFORM_API Win32Application : public Application
		{
		public:
			Win32Application();
			~Win32Application()override;
			int Run()override;
		protected:
			SharedWindowPtr CreateMainWindow()override;
			UniqueRendererPtr CreateRenderer()override;
		};
	}
}
