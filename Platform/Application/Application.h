#pragma once
#include <memory>
#include "ECS/Event.h"
#include "FileSystem.h"
#include "GameWindow.h"
#include "IApplication.h"

namespace Lightning
{
	namespace Render
	{
		class IRenderer;
	}
	namespace App
	{
		using UniqueRendererPtr = std::unique_ptr<Render::IRenderer>;
		using Window::SharedWindowPtr;
		using Foundation::SharedFileSystemPtr;
		
		class Application : public IApplication
		{
		public:
			Application();
			~Application()override;
			void Update()override;
			void Start()override;
			bool IsRunning() override{ return mRunning; }
			int GetExitCode()const override{ return mExitCode; }
		protected:
			virtual void OnWindowIdle(const Window::WindowIdleEvent& event);
			virtual SharedWindowPtr CreateMainWindow() = 0;
			virtual UniqueRendererPtr CreateRenderer() = 0;
			virtual void RegisterWindowHandlers();
			virtual void OnQuit(int exitCode);

			SharedFileSystemPtr mFileSystem;
			UniqueRendererPtr mRenderer;
			SharedWindowPtr mWindow;
			int mExitCode;
			bool mRunning;
		};

	}
}
