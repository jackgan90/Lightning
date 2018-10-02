#pragma once
#include <memory>
#include "platformexportdef.h"
#include "filesystem.h"
#include "windowmanager.h"
#include "timermanager.h"

namespace Lightning
{
	namespace Render
	{
		class IRenderer;
	}
	namespace App
	{
		using Foundation::SharedFileSystemPtr;
		using Foundation::TimerManager;
		using Foundation::ITimer;
		using WindowSystem::WindowManager;
		using WindowSystem::SharedWindowPtr;
		using UniqueRendererPtr = std::unique_ptr<Render::IRenderer>;
		class LIGHTNING_PLATFORM_API IApplication
		{
		public:
			virtual void Start() = 0;
			virtual int Run() = 0;
			virtual ~IApplication() = default;
		};

		class LIGHTNING_PLATFORM_API Application : public IApplication
		{
		public:
			Application();
			~Application()override;
			void Start()override;
		protected:
			virtual void OnWindowIdle(const WindowSystem::WindowIdleParam& param);
			virtual SharedWindowPtr CreateMainWindow() = 0;
			virtual UniqueRendererPtr CreateRenderer() = 0;
			virtual void RegisterWindowHandlers();
			WindowSystem::SharedWindowPtr GetMainWindow() const { return mWindow; }
			//For test
			void GenerateSceneObjects();
			SharedFileSystemPtr mFileSystem;
			UniqueRendererPtr mRenderer;
			std::unique_ptr<WindowManager> mWindowMgr;
			SharedWindowPtr mWindow;
			ITimer *mTimer;
		};

	}
}
