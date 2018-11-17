#pragma once
#include <cstdint>
#include "RefCount.h"

#define WINDOW_MSG_CLASS_HANDLER(EventType, Handler)\
Foundation::EventManager::Instance()->Subscribe<Window::##EventType##>([&](const EventType& event){\
	this->Handler(event);\
})

namespace Lightning
{
	namespace Window
	{
		class IWindow : public Plugins::RefCount
		{
		public:
			virtual bool Show(bool show) = 0;
			virtual void Update() = 0;
			virtual std::uint32_t GetWidth()const = 0;
			virtual std::uint32_t GetHeight()const = 0;
		};

	}

}
