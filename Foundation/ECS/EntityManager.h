#pragma once
#include "Event.h"
#include "Entity.h"
#include "Container.h"
#include "Singleton.h"

namespace Lightning
{
	namespace Foundation
	{
		class EntityManager : public Singleton<EntityManager>
		{
		public:
			template<typename E>
			std::shared_ptr<E> CreateEntity()
			{
				static_assert(std::is_base_of<Entity, E>::value, "E must be a subclass of Entity!");
				auto entity = std::make_shared<E>();
				entity->mID = ++mCurrentEntityID;
				mEntities.insert(std::make_pair(entity->mID, entity));
				return entity;
			}

			template<typename E>
			std::shared_ptr<E> GetEntity(const EntityID& id)
			{
				static const std::shared_ptr<E> sNullPtr;
				auto it = mEntities.find(id);
				if (it != mEntities.end())
					return std::static_pointer_cast<E, Entity>(it->second);
				
				return sNullPtr;
			}

			void RemoveEntity(const EntityID& id)
			{
				auto it = mEntities.find(id);
				if (it != mEntities.end())
				{
					it->second->RemoveAllComponents();
					mIterator = mEntities.erase(it);
					mIteratorValid = false;
				}
			}
			void Update();
		private:
			friend class Singleton<EntityManager>;
			using EntityMap = Container::UnorderedMap<EntityID, EntityPtr>;
			EntityManager();
			EntityID mCurrentEntityID;
			EntityMap mEntities;
			EntityMap::iterator mIterator;
			bool mIteratorValid;
		};
	}
}
