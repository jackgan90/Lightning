#pragma once
#include <cstdint>
#include "IRefObject.h"

#define WINDOW_MSG_CLASS_HANDLER(EventMgr, EventType, Handler)\
EventMgr->Subscribe(EventType, [&](const Foundation::IEvent& event){\
	this->Handler(event);\
})

namespace Lightning
{
	namespace Window
	{
		class IWindow : public Plugins::IRefObject
		{
		public:
			virtual bool Show(bool show) = 0;
			virtual void Update() = 0;
			virtual std::uint32_t GetWidth()const = 0;
			virtual std::uint32_t GetHeight()const = 0;
		};

	}

}
