#pragma once
#include <cstdint>
#include <memory>
#include "foundationexportdef.h"
#include "common.h"

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
		};

		template<typename C>
		using ComponentPtr = std::shared_ptr<C>;
	}
}
