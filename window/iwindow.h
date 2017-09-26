#pragma once
#include <memory>
#include "windowexportdef.h"
#include "iwindownativehandle.h"

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
			virtual WINDOWWIDTH GetWidth()const = 0;
			virtual WINDOWHEIGHT GetHeight()const = 0;
			virtual const IWindowNativeHandle* GetNativeHandle()const = 0;
			virtual ~IWindow(){};
		protected:
			IWindow() {};
		};
		using WindowPtr = std::shared_ptr<IWindow>;
	}
}
