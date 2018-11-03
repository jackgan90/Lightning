#pragma once
#include <cstdint>
#include <functional>
#include <typeinfo>
#include "foundationexportdef.h"
#include "common.h"
#include "entity.h"
#include "singleton.h"
#include "container.h"

namespace Lightning
{
	namespace Foundation
	{
		using EventTypeID = std::uint32_t;
		using EventSubscriberID = std::uint64_t;

		class IEvent
		{
		public:
			virtual ~IEvent(){}
		};

		template<typename Derived>
		class Event : public IEvent
		{
		public:
			static EventTypeID GetTypeID()
			{
				static auto typeName = typeid(Derived).name();
				static EventTypeID typeID = Hash(typeName, std::strlen(typeName), 0x84573821);
				return typeID;
			}
		private:
			friend class EventManager;
			using EventSubscriber = std::function<void(const Derived&)>;
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

		class LIGHTNING_FOUNDATION_API EventManager : public Singleton<EventManager>
		{
		public:
			template<typename EventType>
			EventSubscriberID Subscribe(typename EventType::EventSubscriber handler)
			{
				auto id = ++sCurrentID;
				auto& subscribers = sSubscribers[EventType::GetTypeID()];
				subscribers[id] = [=](const IEvent& event) {
					handler(static_cast<const EventType&>(event));
				};
				return id;
			}
			
			template<typename EventType>
			void Unsubscribe(EventSubscriberID subscriberID)
			{
				auto& subscribers = sSubscribers[EventType::GetTypeID()];
				subscribers.erase(subscriberID);
			}

			template<typename EventType>
			void RaiseEvent(const EventType& evt)
			{
				auto& subscribers = sSubscribers[EventType::GetTypeID()];
				for (auto& it : subscribers)
				{
					it.second(evt);
				}
			}
		private:
			using EventSubscribers = container::unordered_map<EventSubscriberID, std::function<void(const IEvent&)>>;
			static EventSubscriberID sCurrentID;
			static container::unordered_map<EventTypeID, EventSubscribers> sSubscribers;
		};

	}
}