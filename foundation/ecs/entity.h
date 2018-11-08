#pragma once
#undef min
#undef max
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <memory>
#include <algorithm>
#include "rttr/type"
#include "container.h"
#include "component.h"

namespace Lightning
{
	namespace Foundation {
		using EntityID = std::uint64_t;
		using EntityFuncID = std::uint64_t;
		class Entity : public std::enable_shared_from_this<Entity>
		{
		public:
			Entity::Entity():mFuncID(0) {}
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
				CallCompAddedFunc(component);
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
					CallCompRemovedFunc(component);
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
						CallCompRemovedFunc(component);
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

			template<typename C>
			inline EntityFuncID RegisterCompRemovedFunc(std::function<void(const std::shared_ptr<C>&)> callback)
			{
				auto id = ++mFuncID;
				auto cType = rttr::type::get<C>();
				auto func = std::make_shared<CompRemovedFunc>([callback](const ComponentPtr& component) {
					callback(std::static_pointer_cast<C, Component>(component));
				});
				TypedCompRemovedFunc typedFunc{ cType, func };
				mCompRemovedFuncs.insert(std::make_pair(id, typedFunc));
				mCompRemovedFuncList[cType].push_back(func);
				return id;
			}

			inline void UnregisterCompRemovedFunc(const EntityFuncID id)
			{
				auto it = mCompRemovedFuncs.find(id);
				if (it != mCompRemovedFuncs.end())
				{
					auto& funcList = mCompRemovedFuncList[it->second.componentType];
					funcList.erase(std::remove_if(funcList.begin(), funcList.end(), 
						[&](const CompRemovedFuncPtr& ptr) {return ptr.get() == it->second.func.get();}));
					mCompRemovedFuncs.erase(it);
				}
			}
			
			template<typename C>
			inline EntityFuncID RegisterCompAddedFunc(std::function<void(const std::shared_ptr<C>&)> callback)
			{
				auto id = ++mFuncID;
				auto cType = rttr::type::get<C>();
				auto func = std::make_shared<CompAddedFunc>([callback](const ComponentPtr& component) {
					callback(std::static_pointer_cast<C, Component>(component));
				});
				TypedCompAddedFunc typedFunc{ cType, func };
				mCompAddedFuncs.insert(std::make_pair(id, typedFunc));
				mCompAddedFuncList[cType].push_back(func);
				return id;
			}

			inline void UnregisterCompAddedFunc(const EntityFuncID id)
			{
				auto it = mCompAddedFuncs.find(id);
				if (it != mCompAddedFuncs.end())
				{
					auto& funcList = mCompAddedFuncList[it->second.componentType];
					funcList.erase(std::remove_if(funcList.begin(), funcList.end(), 
						[&](const CompAddedFuncPtr& ptr) {return ptr.get() == it->second.func.get();}));
					mCompAddedFuncs.erase(it);
				}
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
			void CallCompAddedFunc(const ComponentPtr& component )
			{
				auto& obj = *component.get();
				auto cType = rttr::type::get(obj);
				for (auto& pFunc : mCompAddedFuncList[cType])
				{
					(*pFunc)(component);
				}
				const auto base_classes = cType.get_base_classes();
				for (auto& base_class : base_classes)
				{
					for (auto& pFunc : mCompAddedFuncList[base_class])
					{
						(*pFunc)(component);
					}
				}
			}

			void CallCompRemovedFunc(const ComponentPtr& component)
			{
				auto& obj = *component.get();
				auto cType = rttr::type::get(obj);
				for (auto& pFunc : mCompRemovedFuncList[cType])
				{
					(*pFunc)(component);
				}
				const auto base_classes = cType.get_base_classes();
				for (auto& base_class : base_classes)
				{
					for (auto& pFunc : mCompRemovedFuncList[base_class])
					{
						(*pFunc)(component);
					}
				}
			}

			using ComponentMap = container::unordered_map<rttr::type, ComponentPtr>;
			using CompAddedFunc = std::function<void(const ComponentPtr&)>;
			using CompRemovedFunc = std::function<void(const ComponentPtr&)>;
			using CompAddedFuncPtr = std::shared_ptr<CompAddedFunc>;
			using CompRemovedFuncPtr = std::shared_ptr<CompRemovedFunc>;
			struct TypedCompAddedFunc
			{
				rttr::type componentType;
				CompAddedFuncPtr func;
			};

			struct TypedCompRemovedFunc
			{
				rttr::type componentType;
				CompRemovedFuncPtr func;
			};

			ComponentMap mComponents;
			ComponentMap::iterator mItComponents;
			container::unordered_map<EntityFuncID, TypedCompRemovedFunc> mCompRemovedFuncs;
			container::unordered_map<rttr::type, container::list<CompRemovedFuncPtr>> mCompRemovedFuncList;
			container::unordered_map<EntityFuncID, TypedCompAddedFunc> mCompAddedFuncs;
			container::unordered_map<rttr::type, container::list<CompAddedFuncPtr>> mCompAddedFuncList;
			EntityID mID;
			EntityFuncID mFuncID;
			RTTR_ENABLE()
		};

		using EntityPtr = std::shared_ptr<Entity>;
	}
}
