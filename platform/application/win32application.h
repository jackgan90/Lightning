#pragma once
#include "platformexportdef.h"
#include "iapplication.h"
#include "iwindow.h"
#include "filesystem.h"
#include "irenderer.h"
#include "memorysystem.h"

namespace LightningGE
{
	namespace App
	{
		using WindowSystem::WindowPtr;
		using memory::BaseMemoryAllocator;
		class LIGHTNINGGE_PLATFORM_API Win32Application : public Application<BaseMemoryAllocator>
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
			bool CreateMainWindow()override;
			bool InitRenderContext() override;
			WindowSystem::WindowPtr GetMainWindow() const override { return m_pWin; }
			void OnWindowIdle(const WindowSystem::WindowIdleParam& param)override;
		private:
			void TryDestroyWindow();
			Foundation::FileSystemPtr m_filesystem;
			WindowPtr m_pWin;
			Renderer::RendererPtr m_renderer;
		};
	}
}