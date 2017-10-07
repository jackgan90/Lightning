#pragma once
#include <memory>
#include <unordered_map>
#include "windowexportdef.h"
#include "iwindownativehandle.h"
#include "windowmessage.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		using WINDOWWIDTH = unsigned int;
		using WINDOWHEIGHT = unsigned int;
		class LIGHTNINGGE_WINDOW_API IWindow
		{
		public:
			friend class WindowManager;
			virtual void destroy() = 0;
			virtual bool Init() = 0;
			virtual bool Show(bool show) = 0;
			virtual void RegisterWindowMessageHandler(WindowMessage msg, WindowMessageHandler handler) = 0;
			virtual WINDOWWIDTH GetWidth()const = 0;
			virtual WINDOWHEIGHT GetHeight()const = 0;
			virtual const WindowNativeHandlePtr GetNativeHandle()const = 0;
			virtual int GetDestroyCode() = 0;
			virtual ~IWindow(){};
		protected:
			IWindow() {};
		};
		using WindowPtr = std::shared_ptr<IWindow>;
	}
}
