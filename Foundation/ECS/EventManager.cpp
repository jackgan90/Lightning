#include "EventManager.h"

namespace Lightning
{
	namespace Foundation
	{
		EventManager::EventManager() : mCurrentID(EventSubscriberID(0))
		{

		}

		EventSubscriberID EventManager::Subscribe(EventType eventType, std::function<void(const IEvent&)> subscriber)
		{
			auto id = ++mCurrentID;
			mEventTypes[id] = eventType;
			auto& subscribers = mSubscribers[eventType];
			subscribers[id] = subscriber;
			return id;
		}

		void EventManager::Unsubscribe(EventSubscriberID subscriberID)
		{
			auto itEventType = mEventTypes.find(subscriberID);
			if (itEventType == mEventTypes.end())
				return;
			auto eventType = itEventType->second;
			auto it = mSubscribers.find(eventType);
			if (it != mSubscribers.end())
			{
				auto& subscribers = it->second;
				auto its = subscribers.find(subscriberID);
				if (its != subscribers.end())
				{
					mItSubscribers[eventType] = subscribers.erase(its);
					mEventTypes.erase(itEventType);
					mItSubscriberValid = false;
				}
			}
		}

		void EventManager::RaiseEvent(const IEvent& event)
		{
			const auto eventType = event.TypeIdentifier();
			auto it = mSubscribers.find(eventType);
			if (it != mSubscribers.end())
			{
				auto& subscribers = it->second;
				for (mItSubscribers[eventType] = subscribers.begin(); mItSubscribers[eventType] != subscribers.end();)
				{
					mItSubscriberValid = true;
					mItSubscribers[eventType]->second(event);
					if (mItSubscriberValid)
						++mItSubscribers[eventType];
				}
			}
		}
	}
}
