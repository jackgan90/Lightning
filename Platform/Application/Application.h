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
			INTERFACECALL ~Application()override;
			void INTERFACECALL Update()override;
			void INTERFACECALL Start()override;
			bool INTERFACECALL IsRunning() override{ return mRunning; }
			int INTERFACECALL GetExitCode()const override{ return mExitCode; }

			//IWindowEventReceiver
			void INTERFACECALL OnWindowCreated(IWindow* window)override;
			void INTERFACECALL OnWindowDestroy(IWindow* window, int exitCode)override;
			void INTERFACECALL OnWindowIdle(IWindow* window)override;
			void INTERFACECALL OnWindowResize(IWindow* window, std::size_t width, std::size_t height)override;
			void INTERFACECALL OnWindowMouseWheel(IWindow* window, int delta, bool isVertical)override;
			void INTERFACECALL OnWindowKeyDown(IWindow* window, VirtualKeyCode keyCode)override;
			void INTERFACECALL OnWindowMouseDown(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)override;
			void INTERFACECALL OnWindowMouseMove(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)override;

		protected:
			virtual void OnQuit(int exitCode);

			IRenderer* mRenderer;
			IWindow* mWindow;
			int mExitCode;
			bool mRunning;
		};

	}
}
