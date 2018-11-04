#pragma once
#include <cstdint>
#include <memory>
#include "foundationexportdef.h"
#include "common.h"
#include "entity.h"

namespace Lightning
{
	namespace Foundation
	{
		using ComponentTypeID = std::uint32_t;
		class IComponent
		{
		public:
			virtual ~IComponent() {}
		};

		template<typename T>
		class Component : public IComponent
		{
		public:
			static const ComponentTypeID GetTypeID()
			{
				static auto typeName = typeid(T).name();
				static EventTypeID typeID = Hash(typeName, std::strlen(typeName), 0x30954821);
				return typeID;
			}

			void Remove()
			{
				if (auto owner = mOwner.lock())
				{
					owner->RemoveComponent<T>();
				}
			}
		protected:
			friend class Entity;
			std::weak_ptr<Entity> mOwner;
		};

		template<typename C>
		using ComponentPtr = std::shared_ptr<C>;
	}
}
