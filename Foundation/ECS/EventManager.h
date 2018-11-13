#pragma once
#include "Event.h"
#include "FoundationExportDef.h"
#include "rttr/type"
#include "Singleton.h"

namespace Lightning
{
	namespace Foundation
	{
		class LIGHTNING_FOUNDATION_API EventManager : public Singleton<EventManager>
		{
		public:
			template<typename EventType>
			EventSubscriberID Subscribe(std::function<void(const EventType&)> subscriber)
			{
				auto id = ++mCurrentID;
				const auto eType = rttr::type::get<EventType>();
				auto& subscribers = mSubscribers[eType];
				subscribers[id] = [=](const Event& event) {
					subscriber(static_cast<const EventType&>(event));
				};
				return id;
			}
			
			template<typename EventType>
			void Unsubscribe(EventSubscriberID subscriberID)
			{
				static_assert(std::is_base_of<Event, EventType>, "Unsubscribe EventType must be a subclass of Event.");
				const auto eType = rttr::type::get<EventType>();
				auto it = mSubscribers.find(eType);
				if (it != mSubscribers.end())
				{
					auto its = it->second.find(subscriberID);
					if (its != it->second.end())
					{
						mItSubscriber = it->second.erase(its);
						mItSubscriberValid = false;
					}
				}
			}

			template<typename EventType>
			void RaiseEvent(const EventType& evt)
			{
				static_assert(std::is_base_of<Event, EventType>::value, "Raised event type must be a subclass of Event");
				const auto eType = rttr::type::get(evt);
				auto it = mSubscribers.find(eType);
				if (it != mSubscribers.end())
				{
					auto& subscribers = it->second;
					for (mItSubscriber = subscribers.begin(); mItSubscriber != subscribers.end();)
					{
						mItSubscriberValid = true;
						mItSubscriber->second(evt);
						if (mItSubscriberValid)
							++mItSubscriber;
					}
				}

				const auto base_classes = eType.get_base_classes();
				for (const auto& Class : base_classes)
				{
					auto it = mSubscribers.find(Class);
					if (it != mSubscribers.end())
					{
						auto& subscribers = it->second;
						for (mItSubscriber = subscribers.begin(); mItSubscriber != subscribers.end();)
						{
							mItSubscriberValid = true;
							mItSubscriber->second(evt);
							if (mItSubscriberValid)
								++mItSubscriber;
						}
					}
				}
			}
		private:
			EventManager();
			friend class Singleton<EventManager>;
			using EventSubscribers = Container::UnorderedMap<EventSubscriberID, EventSubscriber>;
			EventSubscriberID mCurrentID;
			Container::UnorderedMap<rttr::type, EventSubscribers> mSubscribers;
			EventSubscribers::iterator mItSubscriber;
			bool mItSubscriberValid;
		};
	}
}