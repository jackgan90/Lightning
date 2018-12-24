#pragma once
#include <exception>
#include "WindowEvents.h"
#include "Container.h"
#include "IWindow.h"
#include "Logger.h"
#include "RefObject.h"

namespace Lightning
{
	namespace Window
	{
		using Foundation::Container;
		class WindowInitException : public std::exception
		{
		public:
			template<typename... Args>
			WindowInitException(const char*const w, const Args&... args) : exception(w)
			{
				LOG_ERROR(w, args...);
			}
		};

		class GameWindow : public IWindow
		{
		public:
			//Only left for backwards compatibility,will remove later once refactoring is over
			std::uint32_t INTERFACECALL GetWidth()const override{ return mWidth; }
			std::uint32_t INTERFACECALL GetHeight()const override{ return mHeight; }
			bool INTERFACECALL RegisterEventReceiver(IWindowEventReceiver* receiver)override;
			bool INTERFACECALL UnregisterEventReceiver(IWindowEventReceiver* receiver)override;
		protected:
			GameWindow();
			virtual void OnCreated();
			virtual void OnIdle();
			virtual void OnResize(std::size_t width, std::size_t height);
			virtual void OnMouseWheel(int delta, bool isVertical);
			virtual void OnKeyDown(VirtualKeyCode keyCode);
			virtual void OnMouseDown(std::size_t x, std::size_t y, VirtualKeyCode keyCode);
			virtual void OnMouseMove(std::size_t x, std::size_t y, VirtualKeyCode keyCode);
			virtual void OnDestroy(int exitCode);
			Container::Vector<IWindowEventReceiver*> mEventReceivers;
			std::string mCaption;
			std::uint32_t mWidth;
			std::uint32_t mHeight;
			REF_OBJECT_OVERRIDE(GameWindow)
		};
	}
}
