#pragma once
#undef min
#undef max
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <memory>
#include "rttr/type"
#include "container.h"
#include "component.h"

namespace Lightning
{
	namespace Foundation {
		using EntityID = std::uint64_t;

		class Entity : public std::enable_shared_from_this<Entity>
		{
		public:
			Entity::Entity():mRemoved(false){}
			template<typename C, typename... Args>
			std::shared_ptr<C> AddComponent(Args&&... args)
			{
				static_assert(std::is_base_of<Component, C>::value, "C must be a subclass of Component!");
				const auto cType = rttr::type::get<C>();
				assert(mComponents.find(cType) == mComponents.end() && "Duplicate components are not supported!");
				auto component = std::make_shared<C>(std::forward<Args>(args)...);
				component->mOwner = shared_from_this();
				mComponents.emplace(cType, component);

				return component;
			}

			template<typename C>
			void RemoveComponent()
			{
				static_assert(std::is_base_of<Component, C>::value, "C must be a subclass of Component!");
				const auto cType = rttr::type::get<C>();
				auto it = mComponents.find(cType);
				if (it != mComponents.end())
				{
					mComponents.erase(it);
					return;
				}
				//should check subclasses as well for polymorphic case
				const auto derived_classes = cType.get_derived_classes();
				for (const auto& Class : derived_classes)
				{
					it = mComponents.find(Class);
					if (it != mComponents.end())
					{
						mComponents.erase(it);
						break;
					}
				}
			}

			template<typename C>
			void RemoveComponent(const std::shared_ptr<C>& component)
			{
				RemoveComponent<C>();
			}

			template<typename C>
			std::shared_ptr<C> GetComponent()
			{
				static_assert(std::is_base_of<Component, C>::value, "C must be a subclass of Component!");
				static const std::shared_ptr<C> sNullPtr;
				const auto cType = rttr::type::get<C>();
				auto it = mComponents.find(cType);
				if (it != mComponents.end())
					return std::static_pointer_cast<C, Component>(it->second);
				const auto derived_classes = cType.get_derived_classes();
				for (const auto& Class : derived_classes)
				{
					it = mComponents.find(Class);
					if (it != mComponents.end())
					{
						return std::static_pointer_cast<C, Component>(it->second);
					}
				}

				return sNullPtr;
			}

			const EntityID GetID()const
			{
				return mID;
			}
		protected:
			friend class EntityManager;
			container::unordered_map<rttr::type, ComponentPtr> mComponents;
			bool mRemoved;
			EntityID mID;
			RTTR_ENABLE()
		};

		using EntityPtr = std::shared_ptr<Entity>;
	}
}
