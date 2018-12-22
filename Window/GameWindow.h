#pragma once
#include <memory>
#include <unordered_map>
#include <exception>
#include "WindowEvents.h"
#include "IWindow.h"
#include "Logger.h"
#include "ECS/IEventManager.h"
#include "RefObject.h"

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

		class GameWindow : public IWindow
		{
		public:
			//Only left for backwards compatibility,will remove later once refactoring is over
			std::uint32_t INTERFACECALL GetWidth()const override{ return mWidth; }
			std::uint32_t INTERFACECALL GetHeight()const override{ return mHeight; }
		protected:
			GameWindow();
			virtual void OnIdle();
			std::string mCaption;
			std::uint32_t mWidth;
			std::uint32_t mHeight;
			Foundation::IEventManager* mEventMgr;
			REF_OBJECT_OVERRIDE(GameWindow)
		};
	}
}
