#pragma once
#include <memory>
#include "ECS/Event.h"
#include "FileSystem.h"
#include "GameWindow.h"

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
		
		class Application
		{
		public:
			Application();
			virtual ~Application();
			virtual void Update();
			virtual void Start();
			bool IsRunning() { return mRunning; }
			int GetExitCode()const { return mExitCode; }
		protected:
			virtual void OnWindowIdle(const Window::WindowIdleEvent& event);
			virtual SharedWindowPtr CreateMainWindow() = 0;
			virtual UniqueRendererPtr CreateRenderer() = 0;
			virtual void RegisterWindowHandlers();
			virtual void OnQuit(int exitCode);
			//For test
			void GenerateSceneObjects();

			SharedFileSystemPtr mFileSystem;
			UniqueRendererPtr mRenderer;
			SharedWindowPtr mWindow;
			int mExitCode;
			bool mRunning;
		};

	}
}
