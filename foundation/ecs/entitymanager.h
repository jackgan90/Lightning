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
			EntityPtr<E> CreateEntity()
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
				EntityCreated<E> evt(std::static_pointer_cast<E, Entity>(entity));
				EventManager::Instance()->RaiseEvent<EntityCreated<E>>(evt);
				return evt.entity;
			}

			template<typename E>
			EntityPtr<E> GetEntity(const EntityID& id)
			{
				static const EntityPtr<E> sNullPtr;
				auto it = mEntities.find(id);
				if (it != mEntities.end() && !it->second->mRemoved)
					return std::static_pointer_cast<E, Entity>(it->second);
				
				it = mAddingEntities.find(id);
				if(it != mAddingEntities.end() && !it->second->mRemoved)
					return std::static_pointer_cast<E, Entity>(it->second);

				return sNullPtr;

			}

			template<typename E>
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
					EntityRemoved<E> evt(std::static_pointer_cast<E, Entity>(it->second));
					if (mUpdating)
					{
						mRemovingEntities.push_back(id);
					}
					else
					{
						mEntities.erase(it);
					}
					EventManager::Instance()->RaiseEvent<EntityRemoved<E>>(evt);
				}
			}
			void Update();
		private:
			friend class Singleton<EntityManager>;
			EntityManager();
			bool mUpdating;
			EntityID mCurrentEntityID;
			container::unordered_map<EntityID, EntityPtr<Entity>> mEntities;
			container::vector<EntityID> mRemovingEntities;
			container::unordered_map<EntityID, EntityPtr<Entity>> mAddingEntities;
		};
	}
}
