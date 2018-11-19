#pragma once
#include <functional>
#include "IEvent.h"

namespace Lightning
{
	namespace Foundation
	{
		using EventSubscriberID = std::uint64_t;
		using EventSubscriber = std::function<void(const IEvent&)>;
		class IEventManager
		{
		public:
			virtual ~IEventManager() = default;
			//subscribe to specific event type with an event handler
			virtual EventSubscriberID Subscribe(EventType, EventSubscriber) = 0;
			//Unsubscribe to event with subscribe id returned by Subscribe method
			virtual void Unsubscribe(EventSubscriberID id) = 0;
			//Raise event, causing subscribers handle it
			virtual void RaiseEvent(const IEvent& event) = 0;
		};
	}
}