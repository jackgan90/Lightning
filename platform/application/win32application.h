#pragma once
#include "platformexportdef.h"
#include "iapplication.h"
#include "iwindow.h"
#include "filesystem.h"
#include "irendercontext.h"

namespace LightningGE
{
	namespace App
	{
		using WindowSystem::WindowPtr;
		class LIGHTNINGGE_PLATFORM_API Win32Application : public Application
		{
		public:
			Win32Application();
			~Win32Application()override;
			Win32Application(const Win32Application&) = delete;
			Win32Application(Win32Application&&) = delete;
			Win32Application& operator=(const Win32Application&) = delete;
			bool Init()override;
			bool Start()override;
			int Run()override;
			void Quit()override;
		protected:
			bool InitRenderContext() override;
		private:
			void TryDestroyWindow();
			WindowPtr m_pWin;
			Foundation::FileSystemPtr m_filesystem;
			Renderer::RenderContextPtr m_renderContext;
		};
	}
}