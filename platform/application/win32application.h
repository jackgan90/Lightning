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
		class LIGHTNINGGE_PLATFORM_API Win32Application : public Application<Win32Application>
		{
		public:
			Win32Application();
			~Win32Application()override;
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
			WindowPtr m_pWin;
			Renderer::IRenderer* m_renderer;
		};
	}
}