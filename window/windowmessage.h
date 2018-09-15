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
			unsigned int width;
			unsigned int height;
		};

		using WindowMessageHandler = std::function<void(WindowMessage, const WindowMessageParam&)>;

	}
}
