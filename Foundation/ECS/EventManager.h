#pragma once
#include "Singleton.h"
#include "Container.h"
#include "IEvent.h"

namespace Lightning
{
	namespace Foundation
	{
		using EventSubscriber = std::function<void(const IEvent&)>;
		using EventSubscriberID = std::uint64_t;
		class EventManager : public Singleton<EventManager>
		{
		public:
			EventSubscriberID Subscribe(EventType eventType, EventSubscriber subscriber);
			void Unsubscribe(EventSubscriberID subscriberID);
			void RaiseEvent(const IEvent& event);
		private:
			friend class Singleton<EventManager>;
			EventManager();
			using Subscribers = Container::UnorderedMap<EventSubscriberID, EventSubscriber>;
			using EventSubscribers = Container::UnorderedMap<EventType, Subscribers>;
			using EventTypes = Container::UnorderedMap<EventSubscriberID, EventType>;
			using Iterators = Container::UnorderedMap<EventType, Subscribers::iterator>;
			EventTypes mEventTypes;
			EventSubscribers mSubscribers;
			Iterators mItSubscribers;
			bool mItSubscriberValid;
			EventSubscriberID mCurrentID;
		};
	}
}