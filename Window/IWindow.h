#pragma once
#include <cstdint>
#include "cstddef"
#include "IRefObject.h"
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
		struct IWindow : Plugins::IRefObject
		{
			virtual bool INTERFACECALL Show(bool show) = 0;
			virtual void INTERFACECALL Update() = 0;
			virtual std::uint32_t INTERFACECALL GetWidth()const = 0;
			virtual std::uint32_t INTERFACECALL GetHeight()const = 0;
			virtual bool INTERFACECALL RegisterEventReceiver(IWindowEventReceiver* receiver) = 0;
			virtual bool INTERFACECALL UnregisterEventReceiver(IWindowEventReceiver* receiver) = 0;
		};

		struct IWindowEventReceiver
		{
			virtual void INTERFACECALL OnWindowCreated(IWindow* window) = 0;
			virtual void INTERFACECALL OnWindowDestroy(IWindow* window, int exitCode) = 0;
			virtual void INTERFACECALL OnWindowIdle(IWindow* window) = 0;
			virtual void INTERFACECALL OnWindowResize(IWindow* window, std::size_t width, std::size_t height) = 0;
			virtual void INTERFACECALL OnWindowMouseWheel(IWindow* window, int delta, bool isVertical) = 0;
			virtual void INTERFACECALL OnWindowKeyDown(IWindow* window, VirtualKeyCode keyCode) = 0;
			virtual void INTERFACECALL OnWindowMouseDown(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y) = 0;
			virtual void INTERFACECALL OnWindowMouseMove(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y) = 0;
		};


	}

}
