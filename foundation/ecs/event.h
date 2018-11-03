#pragma once
#include <cstdint>
#include <functional>
#include "foundationexportdef.h"
#include "singleton.h"
#include "container.h"

namespace Lightning
{
	namespace Foundation
	{
		using EventTypeID = std::uint64_t;
		using EventSubscriberID = std::uint64_t;
		class LIGHTNING_FOUNDATION_API IEvent
		{
		public:
			virtual ~IEvent() {}
		protected:
			static EventTypeID sEventTypeID;
		};

		template<typename Derived>
		class Event : public IEvent
		{
		public:
			static EventTypeID GetTypeID()
			{
				static const EventTypeID typeID = ++sEventTypeID;
				return typeID;
			}
		private:
			friend class EventManager;
			using EventSubscriber = std::function<void(const Derived&)>;
			using EventSubscribers = container::unordered_map<EventSubscriberID, EventSubscriber>;
			static EventSubscriberID Subscribe(EventSubscriber handler)
			{
				auto id = ++sCurrentID;
				sSubscribers[id] = handler;
				return id;
			}
			static void Unsubscribe(EventSubscriberID subscriberID)
			{
				sSubscribers.erase(subscriberID);
			}

			static void Raise(const Derived& evt)
			{
				for (auto& it : sSubscribers)
				{
					it.second(evt);
				}
			}
			static EventSubscribers sSubscribers;
			static EventSubscriberID sCurrentID;
		};

		template<typename Derived>
		typename Event<Derived>::EventSubscribers Event<Derived>::sSubscribers;

		template<typename Derived>
		EventSubscriberID Event<Derived>::sCurrentID = EventSubscriberID(0);

		class EventManager : public Singleton<EventManager>
		{
		public:
			template<typename EventType>
			EventSubscriberID Subscribe(typename EventType::EventSubscriber handler)
			{
				return EventType::Subscribe(handler);
			}
			
			template<typename EventType>
			void Unsubscribe(EventSubscriberID subscriberID)
			{
				EventType::Unsubscribe(subscriberID);
			}

			template<typename EventType>
			void RaiseEvent(const EventType& evt)
			{
				EventType::Raise(evt);
			}
		};
	}
}