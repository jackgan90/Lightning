#pragma once
#undef min
#undef max
#include <functional>
#include "IWindow.h"
#include "ECS/Event.h"

namespace Lightning
{
	namespace Window
	{
		class IWindow;
		using Foundation::Event;
		enum VirtualKeyCode
		{
			//alphabet
			VK_A = 1,
			VK_B, VK_C, VK_D, VK_E, VK_F, VK_G, VK_H, VK_I, VK_J, VK_K, VK_L, VK_M, VK_N,
			VK_O, VK_P, VK_Q, VK_R, VK_S, VK_T, VK_U, VK_V, VK_W, VK_X, VK_Y, VK_Z,
			//special keys
			VK_CONTROL_MASK = 0xffffff00,
			VK_CTRL = 0x100,
			VK_MOUSELBUTTON = 0x200,
			VK_MOUSEMBUTTON = 0x400,
			VK_MOUSERBUTTON = 0x800,
			VK_SHIFTBUTTON = 0x1000,
		};

		struct WindowEvent : Event
		{
			WindowEvent(const IWindow* ptr) : pWindow(ptr){}
			const IWindow* pWindow;
			RTTR_ENABLE(Event)
		};

		struct WindowCreatedEvent : WindowEvent
		{
			WindowCreatedEvent(const IWindow* ptr) : WindowEvent(ptr){}
			RTTR_ENABLE(WindowEvent)
		};

		struct WindowDestroyedEvent : WindowEvent
		{
			WindowDestroyedEvent(const IWindow* ptr) : WindowEvent(ptr){}
			long long exitCode;
			RTTR_ENABLE(WindowEvent)
		};

		struct WindowIdleEvent : WindowEvent
		{
			WindowIdleEvent(const IWindow* ptr) : WindowEvent(ptr){}
			RTTR_ENABLE(WindowEvent)
		};

		struct WindowResizeEvent : WindowEvent
		{
			WindowResizeEvent(const IWindow* ptr) : WindowEvent(ptr){}
			unsigned int width;
			unsigned int height;
			RTTR_ENABLE(WindowEvent)
		};

		struct MouseWheelEvent : WindowEvent
		{
			MouseWheelEvent(const IWindow* ptr) : WindowEvent(ptr){}
			int wheel_delta;
			bool is_vertical;
			RTTR_ENABLE(WindowEvent)
		};

		struct KeyEvent : WindowEvent
		{
			KeyEvent(const IWindow* ptr) : WindowEvent(ptr){}
			VirtualKeyCode code;
			RTTR_ENABLE(WindowEvent)
		};

		struct MouseDownEvent : WindowEvent
		{
			MouseDownEvent(const IWindow* ptr) : WindowEvent(ptr){}
			std::size_t x;
			std::size_t y;
			VirtualKeyCode pressedKey;
			RTTR_ENABLE(WindowEvent)
		};

		struct MouseMoveEvent : WindowEvent
		{
			MouseMoveEvent(const IWindow* ptr) : WindowEvent(ptr){}
			std::size_t x;
			std::size_t y;
			VirtualKeyCode pressedKey;
			RTTR_ENABLE(WindowEvent)
		};
	}
}
