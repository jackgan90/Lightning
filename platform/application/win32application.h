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
		using WindowSystem::WindowPtr;
		using WindowSystem::WindowManager;
		using memory::BaseMemoryAllocator;
		class LIGHTNINGGE_PLATFORM_API Win32Application : public Application
		{
		public:
			Win32Application();
			~Win32Application()override;
			bool Init()override;
			void Start()override;
			int Run()override;
			void Quit()override;
			WindowManager* GetWindowManager()const override
			{
				return m_windowMgr;
			}
		protected:
			bool CreateMainWindow()override;
			void InitRenderContext() override;
			WindowSystem::WindowPtr GetMainWindow() const override { return m_window; }
			void OnWindowIdle(const WindowSystem::WindowIdleParam& param)override;
		private:
			void TryDestroyWindow();
			FileSystemPtr m_fs;
			WindowManager* m_windowMgr;
			WindowPtr m_window;
			std::shared_ptr<Renderer::IRenderer> m_renderer;
			
		};
	}
}