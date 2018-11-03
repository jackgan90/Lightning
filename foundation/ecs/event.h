#pragma once
#include <cstdint>
#include <functional>
#include "foundationexportdef.h"
#include "entity.h"
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

		//Built-in events
		template<typename E>
		struct EntityCreated : Event<EntityCreated<E>>
		{
			EntityCreated(const EntityPtr<E>& e) : entity(e){}
			const EntityPtr<E>& entity;
		};

		template<typename E>
		struct EntityRemoved : Event<EntityRemoved<E>>
		{
			EntityRemoved(const EntityPtr<E>& e) : entity(e){}
			const EntityPtr<E>& entity;
		};

		template<typename C>
		struct ComponentAdded : Event<ComponentAdded<C>>
		{
			ComponentAdded(const EntityPtr<Entity>& _entity, const ComponentPtr<C>& _component) 
				: component(_component), entity(_entity){}
			const EntityPtr<Entity>& entity;
			const ComponentPtr<C>& component;
		};

		template<typename C>
		struct ComponentRemoved : Event<ComponentRemoved<C>>
		{
			ComponentRemoved(const EntityPtr<Entity>& _entity, const ComponentPtr<C>& _component) 
				: component(_component), entity(_entity){}
			const EntityPtr<Entity>& entity;
			const ComponentPtr<C>& component;
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