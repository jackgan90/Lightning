#pragma once
#include <memory>
#include "ECS/Event.h"
#include "IFileSystem.h"
#include "IWindow.h"
#include "WindowEvents.h"
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
		using Render::IRenderer;
		using Foundation::IFileSystem;
		using Window::IWindow;
		
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
			virtual IRenderer* CreateRenderer();
			virtual IWindow* CreateMainWindow() = 0;
			virtual void RegisterWindowHandlers();
			virtual void OnQuit(int exitCode);

			IFileSystem* mFileSystem;
			IRenderer* mRenderer;
			IWindow* mWindow;
			int mExitCode;
			bool mRunning;
		};

	}
}
