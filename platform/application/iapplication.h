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
		using Foundation::FileSystemPtr;
		using WindowSystem::WindowManager;
		using Renderer::IRenderer;
		class LIGHTNINGGE_PLATFORM_API IApplication
		{
		public:
			virtual void Start() = 0;
			virtual int Run() = 0;
			virtual WindowManager* GetWindowManager()const = 0;
			virtual ~IApplication(){}
		};

		class LIGHTNINGGE_PLATFORM_API Application : public IApplication
		{
		public:
			Application();
			~Application()override;
			void Start()override;
		protected:
			virtual void OnWindowIdle(const WindowSystem::WindowIdleParam& param);
			virtual bool CreateMainWindow() = 0;
			virtual IRenderer* CreateRenderer() = 0;
			virtual void RegisterWindowHandlers();
			virtual WindowSystem::WindowPtr GetMainWindow() const = 0;
			FileSystemPtr m_fs;
			IRenderer* m_renderer;
		};

	}
}
