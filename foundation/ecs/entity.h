#pragma once
#include <cstdint>
#include <type_traits>
#include <cassert>
#include "foundationexportdef.h"
#include "container.h"
#include "component.h"

namespace Lightning
{
	namespace Foundation {
		using EntityID = std::uint64_t;
		class Entity
		{
		public:
			Entity::Entity():mRemoved(false){}
			template<typename C, typename... Args>
			ComponentPtr<C> AddComponent(Args&&... args)
			{
				static_assert(std::is_base_of<IComponent, C>::value, "C must be a subclass of IComponent!");
				auto typeID = C::GetTypeID();
				assert(mComponents.find(typeID) == mComponents.end() && "Duplicate components are not supported!");
				auto component = std::make_shared<C>(std::forward<Args>(args)...);
				mComponents.emplace(typeID, component);
				return std::static_pointer_cast<C, IComponent>(component);
			}

			template<typename C>
			void RemoveComponent()
			{
				static_assert(std::is_base_of<IComponent, C>::value, "C must be a subclass of IComponent!");
				auto typeID = C::GetTypeID();
				mComponents.erase(typeID);
			}

			template<typename C>
			ComponentPtr<C> GetComponent()
			{
				static_assert(std::is_base_of<IComponent, C>::value, "C must be a subclass of IComponent!");
				static ComponentPtr<C> sNullPtr;
				auto it = mComponents.find(C::GetTypeID());
				if (it != mComponents.end())
					return std::static_pointer_cast<C, IComponent>(it->second);
				return sNullPtr;
			}

			EntityID GetID()
			{
				return mID;
			}
		protected:
			friend class EntityManager;
			container::unordered_map<ComponentTypeID, ComponentPtr<IComponent>> mComponents;
			bool mRemoved;
			EntityID mID;
		};

		template<typename E>
		using EntityPtr = std::shared_ptr<E>;
	}
}
