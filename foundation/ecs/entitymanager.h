#pragma once
#include "foundationexportdef.h"
#include "event.h"
#include "entity.h"
#include "container.h"
#include "singleton.h"

namespace Lightning
{
	namespace Foundation
	{
		class LIGHTNING_FOUNDATION_API EntityManager : public Singleton<EntityManager>
		{
		public:
			template<typename E>
			std::shared_ptr<E> CreateEntity()
			{
				static_assert(std::is_base_of<Entity, E>::value, "E must be a subclass of Entity!");
				auto entity = std::make_shared<E>();
				entity->mID = ++mCurrentEntityID;
				auto it = mEntities.insert(std::make_pair(entity->mID, entity));
				if (it.second)
				{
					mIterator = it.first;
				}
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
				}
			}
			void Update();
		private:
			friend class Singleton<EntityManager>;
			using EntityMap = container::unordered_map<EntityID, EntityPtr>;
			EntityManager();
			EntityID mCurrentEntityID;
			EntityMap mEntities;
			EntityMap::iterator mIterator;
		};
	}
}
