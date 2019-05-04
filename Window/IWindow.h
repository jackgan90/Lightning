#pragma once
#include <cstdint>
#include "cstddef"
#include "WindowEvents.h"

#define WINDOW_MSG_CLASS_HANDLER(EventType, Handler)\
Lightning::Foundation::EventManager::Instance()->Subscribe(EventType, [&](const Foundation::IEvent& event){\
	this->Handler(event);\
})

namespace Lightning
{
	namespace Window
	{
		struct IWindowEventReceiver;
		struct IWindow
		{
			virtual ~IWindow() = default;
			virtual bool Show(bool show) = 0;
			virtual void Tick() = 0;
			virtual std::uint32_t GetWidth()const = 0;
			virtual std::uint32_t GetHeight()const = 0;
			virtual bool RegisterEventReceiver(IWindowEventReceiver* receiver) = 0;
			virtual bool UnregisterEventReceiver(IWindowEventReceiver* receiver) = 0;
		};

		struct IWindowEventReceiver
		{
			virtual void OnWindowCreated(IWindow* window) = 0;
			virtual void OnWindowDestroy(IWindow* window, int exitCode) = 0;
			virtual void OnWindowIdle(IWindow* window) = 0;
			virtual void OnWindowResize(IWindow* window, std::size_t width, std::size_t height) = 0;
			virtual void OnWindowMouseWheel(IWindow* window, int delta, bool isVertical) = 0;
			virtual void OnWindowKeyDown(IWindow* window, VirtualKeyCode keyCode) = 0;
			virtual void OnWindowMouseDown(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y) = 0;
			virtual void OnWindowMouseMove(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y) = 0;
		};


	}

}
