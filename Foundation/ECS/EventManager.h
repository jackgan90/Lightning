#pragma once
#include <unordered_map>
#include <functional>
#include "Singleton.h"
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
			using Subscribers = std::unordered_map<EventSubscriberID, EventSubscriber>;
			using EventSubscribers = std::unordered_map<EventType, Subscribers>;
			using EventTypes = std::unordered_map<EventSubscriberID, EventType>;
			using Iterators = std::unordered_map<EventType, Subscribers::iterator>;
			EventTypes mEventTypes;
			EventSubscribers mSubscribers;
			Iterators mItSubscribers;
			bool mItSubscriberValid;
			EventSubscriberID mCurrentID;
		};
	}
}