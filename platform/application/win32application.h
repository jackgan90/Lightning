#pragma once
#include "platformexportdef.h"
#include "iapplication.h"
#include "iwindow.h"
#include "windowmanager.h"
#include "filesystem.h"
#include "irenderer.h"
#include "memorysystem.h"

namespace LightningGE
{
	namespace App
	{
		using WindowSystem::SharedWindowPtr;
		using WindowSystem::WindowManager;
		using Render::UniqueRendererPtr;
		class LIGHTNINGGE_PLATFORM_API Win32Application : public Application
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