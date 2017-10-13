#pragma once
#include <memory>
#include <unordered_map>
#include <exception>
#include "windowexportdef.h"
#include "windowmessage.h"
#include "logger.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		class WindowInitException : public std::exception
		{
		public:
			template<typename... Args>
			WindowInitException(const char*const w, const Args&... args) : exception(w)
			{
				Foundation::logger.Log(Foundation::LogLevel::Error, w, args...);
			}
		};
		typedef unsigned int WINDOWWIDTH;
		typedef unsigned int WINDOWHEIGHT;
		class LIGHTNINGGE_WINDOW_API IWindow
		{
		public:
			friend class WindowManager;
			virtual bool Show(bool show) = 0;
			virtual void RegisterWindowMessageHandler(WindowMessage msg, WindowMessageHandler handler) = 0;
			virtual WINDOWWIDTH GetWidth()const = 0;
			virtual WINDOWHEIGHT GetHeight()const = 0;
			virtual int GetDestroyCode() = 0;
			virtual ~IWindow() = default;
		protected:
			IWindow() {};
		};
		typedef std::shared_ptr<IWindow> SharedWindowPtr;
	}
}
