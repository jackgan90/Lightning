#pragma once
#include "event.h"
#include "foundationexportdef.h"
#include "rttr/type"
#include "singleton.h"

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
					it->second.erase(subscriberID);
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
					for (auto& subscriber : it->second)
					{
						subscriber.second(evt);
					}
				}

				const auto base_classes = eType.get_base_classes();
				for (const auto& Class : base_classes)
				{
					auto it = mSubscribers.find(Class);
					if (it != mSubscribers.end())
					{
						for (auto& subscriber : it->second)
						{
							subscriber.second(evt);
						}
					}
				}
			}
		private:
			EventManager();
			friend class Singleton<EventManager>;
			using EventSubscribers = container::unordered_map<EventSubscriberID, EventSubscriber>;
			EventSubscriberID mCurrentID;
			container::unordered_map<rttr::type, EventSubscribers> mSubscribers;
		};
	}
}