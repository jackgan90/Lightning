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
		using EntityCallbackID = std::uint64_t;
		class Entity : public std::enable_shared_from_this<Entity>
		{
		public:
			using ComponentAddedCallback = std::function<void(const ComponentPtr&)>;
			using ComponentRemovedCallback = std::function<void(const ComponentPtr&)>;
			Entity::Entity():mCallbackID(0) {}
			template<typename C, typename... Args>
			std::shared_ptr<C> AddComponent(Args&&... args)
			{
				static_assert(std::is_base_of<Component, C>::value, "C must be a subclass of Component!");
				const auto cType = rttr::type::get<C>();
				assert(mComponents.find(cType) == mComponents.end() && "Duplicate components are not supported!");
				auto component = std::make_shared<C>(std::forward<Args>(args)...);
				component->mOwner = shared_from_this();
				auto insert_res = mComponents.insert(std::make_pair(cType, component));
				if (insert_res.second)
				{
					mItComponents = insert_res.first;
				}
				InvokeComponentAddedCallbacks(component);
				return component;
			}

			template<typename C>
			void RemoveComponent()
			{
				static_assert(std::is_base_of<Component, C>::value, "C must be a subclass of Component!");
				const auto cType = rttr::type::get<C>();
				RemoveComponent(cType);
			}

			void RemoveComponent(const rttr::type& cType)
			{
				auto it = mComponents.find(cType);
				if (it != mComponents.end())
				{
					auto component = it->second;
					mItComponents = mComponents.erase(it);
					InvokeComponentRemovedCallbacks(component);
					return;
				}
				//should check subclasses as well for polymorphic case
				const auto derived_classes = cType.get_derived_classes();
				for (const auto& Class : derived_classes)
				{
					it = mComponents.find(Class);
					if (it != mComponents.end())
					{
						auto component = it->second;
						mItComponents = mComponents.erase(it);
						InvokeComponentRemovedCallbacks(component);
						break;
					}
				}
			}

			template<typename C>
			void RemoveComponent(const std::shared_ptr<C>& component)
			{
				auto pComponent = component.get();
				RemoveComponent(rttr::type::get(*pComponent));
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

			inline EntityCallbackID RegisterComponentRemovedCallback(ComponentRemovedCallback callback)
			{
				auto id = ++mCallbackID;
				mCompRemovedCallbacks.emplace(id, callback);
				return id;
			}

			inline void UnregisterComponentRemovedCallback(const EntityCallbackID id)
			{
				mCompRemovedCallbacks.erase(id);
			}

			inline EntityCallbackID RegisterComponentAddedCallback(ComponentAddedCallback callback)
			{
				auto id = ++mCallbackID;
				mCompAddedCallbacks.emplace(id, callback);
				return id;
			}

			inline void UnregisterComponentAddedCallback(const EntityCallbackID id)
			{
				mCompAddedCallbacks.erase(id);
			}

			const EntityID GetID()const
			{
				return mID;
			}

			void RemoveAllComponents()
			{
				for (mItComponents = mComponents.begin();mItComponents != mComponents.end();)
				{
					auto it = mItComponents;
					RemoveComponent(mItComponents->second);
					if (mItComponents != mComponents.end() && it == mItComponents)
						++mItComponents;
				}
			}
		protected:
			friend class EntityManager;
			template<typename C>
			void InvokeComponentAddedCallbacks(const std::shared_ptr<C>& component )
			{
				if (mCompAddedCallbacks.empty())
					return;
				for (auto& callback : mCompAddedCallbacks)
				{
					callback.second(component);
				}
			}

			template<typename C>
			void InvokeComponentRemovedCallbacks(const std::shared_ptr<C>& component)
			{
				if (mCompRemovedCallbacks.empty())
					return;
				for (auto& callback : mCompRemovedCallbacks)
				{
					callback.second(component);
				}
			}

			using ComponentMap = container::unordered_map<rttr::type, ComponentPtr>;
			ComponentMap mComponents;
			ComponentMap::iterator mItComponents;
			container::unordered_map<EntityCallbackID, ComponentRemovedCallback> mCompRemovedCallbacks;
			container::unordered_map<EntityCallbackID, ComponentAddedCallback> mCompAddedCallbacks;
			EntityID mID;
			EntityCallbackID mCallbackID;
			RTTR_ENABLE()
		};

		using EntityPtr = std::shared_ptr<Entity>;
	}
}
