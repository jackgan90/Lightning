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
				if (mUpdating)
				{
					mAddingEntities.insert(std::make_pair(entity->mID, entity));
				}
				else
				{
					mEntities[entity->mID] = entity;
				}

				return entity;
			}

			template<typename E>
			std::shared_ptr<E> GetEntity(const EntityID& id)
			{
				static const std::shared_ptr<E> sNullPtr;
				auto it = mEntities.find(id);
				if (it != mEntities.end() && !it->second->mRemoved)
					return std::static_pointer_cast<E, Entity>(it->second);
				
				it = mAddingEntities.find(id);
				if(it != mAddingEntities.end() && !it->second->mRemoved)
					return std::static_pointer_cast<E, Entity>(it->second);

				return sNullPtr;
			}

			void RemoveEntity(const EntityID& id)
			{
				auto it = mEntities.find(id);
				if (it != mEntities.end())
				{
					if (it->second->mRemoved)
					{
						return;
					}
					it->second->mRemoved = true;
					if (mUpdating)
					{
						mRemovingEntities.push_back(id);
					}
					else
					{
						it->second->RemoveAllComponents();
						mEntities.erase(it);
					}
				}
			}
			void Update();
		private:
			friend class Singleton<EntityManager>;
			EntityManager();
			bool mUpdating;
			EntityID mCurrentEntityID;
			container::unordered_map<EntityID, EntityPtr> mEntities;
			container::vector<EntityID> mRemovingEntities;
			container::unordered_map<EntityID, EntityPtr> mAddingEntities;
		};
	}
}
