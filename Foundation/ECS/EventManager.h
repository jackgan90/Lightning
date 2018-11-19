#pragma once
#include "IEventManager.h"
#include "Singleton.h"
#include "Container.h"

namespace Lightning
{
	namespace Foundation
	{
		class EventManager : public IEventManager, public Singleton<EventManager>
		{
		public:
			EventSubscriberID Subscribe(EventType eventType, std::function<void(const IEvent&)> subscriber)override;
			void Unsubscribe(EventSubscriberID subscriberID)override;
			void RaiseEvent(const IEvent& event)override;
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