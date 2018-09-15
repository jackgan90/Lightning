#pragma once
#include <memory>
#include "windowexportdef.h"

namespace Lightning
{
	namespace WindowSystem
	{
		enum NativeHandleIdentifier
		{
			IDENTIFIER_WINDOWS_HANDLE,
		};
		class LIGHTNING_WINDOW_API IWindowNativeHandle
		{
		public:
			virtual bool operator==(const IWindowNativeHandle& handle) = 0;
			virtual ~IWindowNativeHandle(){}
			virtual NativeHandleIdentifier GetIdentifier()const = 0;
		};
		typedef std::shared_ptr<IWindowNativeHandle> WindowNativeHandlePtr;
	}
}
