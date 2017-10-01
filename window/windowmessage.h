#pragma once
#include "iwindownativehandle.h"
#include <functional>

namespace LightningGE
{
	namespace WindowSystem
	{
		enum WindowMessage
		{
			MESSAGE_CREATED,
			MESSAGE_DESTROYED,
			MESSAGE_IDLE,
			MESSAGE_RESIZE,
		};

		struct WindowMessageParam
		{

		};

		struct WindowCreatedParam : WindowMessageParam
		{
			WindowNativeHandlePtr nativeHandle;
		};

		struct WindowDestroyedParam : WindowMessageParam
		{
			WindowNativeHandlePtr nativeHandle;
		};

		struct WindowIdleParam : WindowMessageParam
		{
			WindowIdleParam(const WindowNativeHandlePtr ptr) : nativeHandle(ptr){}
			WindowNativeHandlePtr nativeHandle;
		};

		struct WindowResizeParam : WindowMessageParam
		{
			WindowNativeHandlePtr nativeHandle;
			unsigned int width;
			unsigned int height;
		};

		typedef std::function<void(WindowMessage, const WindowMessageParam&)> WindowMessageHandler;

	}
}
