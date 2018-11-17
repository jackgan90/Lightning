#pragma once
#include <memory>
#include <unordered_map>
#include <exception>
#include "WindowExportDef.h"
#include "WindowEvents.h"
#include "IWindow.h"
#include "Logger.h"

namespace Lightning
{
	namespace Window
	{
		class WindowInitException : public std::exception
		{
		public:
			template<typename... Args>
			WindowInitException(const char*const w, const Args&... args) : exception(w)
			{
				LOG_ERROR(w, args...);
			}
		};

		class LIGHTNING_WINDOW_API GameWindow : public IWindow
		{
		public:
			//Only left for backwards compatibility,will remove later once refactoring is over
			std::uint32_t GetWidth()const override{ return mWidth; }
			std::uint32_t GetHeight()const override{ return mHeight; }
		protected:
			GameWindow();
			virtual void OnIdle();
			std::string mCaption;
			std::uint32_t mWidth;
			std::uint32_t mHeight;
		};
	}
}
