#pragma once
#include <memory>
#include <unordered_map>
#include <exception>
#include "WindowExportDef.h"
#include "WindowEvents.h"
#include "Logger.h"

#define WINDOW_MSG_CLASS_HANDLER(EventType, Handler)\
Foundation::EventManager::Instance()->Subscribe<Window::##EventType##>([&](const EventType& event){\
	this->Handler(event);\
})

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

		class LIGHTNING_WINDOW_API GameWindow
		{
		public:
			virtual bool Show(bool show) = 0;
			virtual ~GameWindow();
			virtual void Update() = 0;
			//Only left for backwards compatibility,will remove later once refactoring is over
			std::uint32_t GetWidth()const { return mWidth; }
			std::uint32_t GetHeight()const { return mHeight; }
		protected:
			GameWindow();
			virtual void OnIdle();
			std::string mCaption;
			std::uint32_t mWidth;
			std::uint32_t mHeight;
		};
		using SharedWindowPtr = std::shared_ptr<GameWindow>;
	}
}
