#pragma once
#include <memory>
#include "IWindow.h"
#include "IApplication.h"
#include "IRenderer.h"

namespace Lightning
{
	namespace App
	{
		using Render::IRenderer;
		using Window::IWindow;
		using Window::IWindowEventReceiver;
		using Window::VirtualKeyCode;
		class Application : public IApplication, public IWindowEventReceiver
		{
		public:
			Application();
			~Application()override;
			void Tick()override;
			void Start()override;
			bool IsRunning() override{ return mRunning; }
			int GetExitCode()const override{ return mExitCode; }

			//IWindowEventReceiver
			void OnWindowCreated(IWindow* window)override;
			void OnWindowDestroy(IWindow* window, int exitCode)override;
			void OnWindowIdle(IWindow* window)override;
			void OnWindowResize(IWindow* window, std::size_t width, std::size_t height)override;
			void OnWindowMouseWheel(IWindow* window, int delta, bool isVertical)override;
			void OnWindowKeyDown(IWindow* window, VirtualKeyCode keyCode)override;
			void OnWindowMouseDown(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)override;
			void OnWindowMouseMove(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)override;

		protected:
			virtual void OnQuit(int exitCode);

			IRenderer* mRenderer;
			IWindow* mWindow;
			int mExitCode;
			bool mRunning;
		};

	}
}
