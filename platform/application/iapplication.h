#pragma once
#include <memory>
#include "platformexportdef.h"
#include "filesystem.h"
#include "windowmanager.h"
#include "irenderer.h"


namespace LightningGE
{
	namespace App
	{
		using Foundation::SharedFileSystemPtr;
		using WindowSystem::WindowManager;
		using WindowSystem::WindowPtr;
		using Renderer::UniqueRendererPtr;
		class LIGHTNINGGE_PLATFORM_API IApplication
		{
		public:
			virtual void Start() = 0;
			virtual int Run() = 0;
			virtual ~IApplication() = default;
		};

		class LIGHTNINGGE_PLATFORM_API Application : public IApplication
		{
		public:
			Application();
			~Application()override;
			void Start()override;
		protected:
			virtual void OnWindowIdle(const WindowSystem::WindowIdleParam& param);
			virtual WindowPtr CreateMainWindow() = 0;
			virtual UniqueRendererPtr CreateRenderer() = 0;
			virtual void RegisterWindowHandlers();
			WindowSystem::WindowPtr GetMainWindow() const { return m_window; }
			SharedFileSystemPtr m_fs;
			UniqueRendererPtr m_renderer;
			std::unique_ptr<WindowManager> m_windowMgr;
			WindowPtr m_window;
		};

	}
}
