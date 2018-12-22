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
		enum EventIdentifier
		{
			WINDOW_CREATED_EVENT = 1,
			WINDOW_DESTROYED_EVENT,
			WINDOW_IDLE_EVENT,
			WINDOW_RESIZE_EVENT,
			MOUSE_WHEEL_EVENT,
			KEYBOARD_EVENT,
			MOUSE_DOWN_EVENT,
			MOUSE_MOVE_EVENT
		};

		struct IWindow;
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
			WindowEvent(Foundation::EventType type,const IWindow* ptr) : Event(type),pWindow(ptr){}
			const IWindow* pWindow;
		};

		struct WindowCreatedEvent : WindowEvent
		{
			WindowCreatedEvent(const IWindow* ptr) : WindowEvent(WINDOW_CREATED_EVENT, ptr){}
		};

		struct WindowDestroyedEvent : WindowEvent
		{
			WindowDestroyedEvent(const IWindow* ptr) : WindowEvent(WINDOW_DESTROYED_EVENT, ptr){}
			long long exitCode;
		};

		struct WindowIdleEvent : WindowEvent
		{
			WindowIdleEvent(const IWindow* ptr) : WindowEvent(WINDOW_IDLE_EVENT, ptr){}
		};

		struct WindowResizeEvent : WindowEvent
		{
			WindowResizeEvent(const IWindow* ptr) : WindowEvent(WINDOW_RESIZE_EVENT, ptr){}
			unsigned int width;
			unsigned int height;
		};

		struct MouseWheelEvent : WindowEvent
		{
			MouseWheelEvent(const IWindow* ptr) : WindowEvent(MOUSE_WHEEL_EVENT, ptr){}
			int wheel_delta;
			bool is_vertical;
		};

		struct KeyEvent : WindowEvent
		{
			KeyEvent(const IWindow* ptr) : WindowEvent(KEYBOARD_EVENT, ptr){}
			VirtualKeyCode code;
		};

		struct MouseDownEvent : WindowEvent
		{
			MouseDownEvent(const IWindow* ptr) : WindowEvent(MOUSE_DOWN_EVENT, ptr){}
			std::size_t x;
			std::size_t y;
			VirtualKeyCode pressedKey;
		};

		struct MouseMoveEvent : WindowEvent
		{
			MouseMoveEvent(const IWindow* ptr) : WindowEvent(MOUSE_MOVE_EVENT, ptr){}
			std::size_t x;
			std::size_t y;
			VirtualKeyCode pressedKey;
		};
	}
}
