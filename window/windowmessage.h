#pragma once
#include "iwindow.h"
#include <functional>

namespace LightningGE
{
	namespace WindowSystem
	{
		class IWindow;
		enum WindowMessage
		{
			MESSAGE_CREATED,
			MESSAGE_DESTROYED,
			MESSAGE_IDLE,
			MESSAGE_RESIZE,
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

		typedef std::function<void(WindowMessage, const WindowMessageParam&)> WindowMessageHandler;

	}
}
