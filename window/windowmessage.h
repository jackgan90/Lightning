#pragma once
#include <functional>
#include "iwindow.h"

namespace Lightning
{
	namespace WindowSystem
	{
		class IWindow;
		enum class WindowMessage
		{
			CREATED,
			DESTROYED,
			IDLE,
			RESIZE,
			MOUSE_WHEEL,
		};

		struct WindowMessageParam
		{
			WindowMessageParam(const IWindow* ptr):pWindow(ptr){}
			const IWindow* pWindow;
		};

		struct WindowCreatedParam : WindowMessageParam
		{
		};

		struct WindowDestroyedParam : WindowMessageParam
		{
		};

		struct WindowIdleParam : WindowMessageParam
		{
			WindowIdleParam(const IWindow* ptr) : WindowMessageParam(ptr){}
		};

		struct WindowResizeParam : WindowMessageParam
		{
			WindowResizeParam(const IWindow* ptr) : WindowMessageParam(ptr){}
			unsigned int width;
			unsigned int height;
		};

		struct MouseWheelParam : WindowMessageParam
		{
			MouseWheelParam(const IWindow* ptr) : WindowMessageParam(ptr){}
			int wheel_delta;
			bool is_vertical;
		};

		using WindowMessageHandler = std::function<void(WindowMessage, const WindowMessageParam&)>;

	}
}
