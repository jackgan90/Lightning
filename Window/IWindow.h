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
		struct IWindow : Plugins::IRefObject
		{
			virtual bool INTERFACECALL Show(bool show) = 0;
			virtual void INTERFACECALL Update() = 0;
			virtual std::uint32_t INTERFACECALL GetWidth()const = 0;
			virtual std::uint32_t INTERFACECALL GetHeight()const = 0;
		};

	}

}
