#pragma once
#include <cstdint>
#include <memory>
#include "foundationexportdef.h"

namespace Lightning
{
	namespace Foundation
	{
		using ComponentTypeID = std::uint64_t;
		class LIGHTNING_FOUNDATION_API IComponent
		{
		public:
			virtual ~IComponent() {}
		protected:
			static ComponentTypeID sTypeID;
		};

		template<typename T>
		class Component : public IComponent
		{
		public:
			static const ComponentTypeID GetTypeID()
			{
				static const ComponentTypeID typeID = ++sTypeID;
				return typeID;
			}
		};

		template<typename C>
		using ComponentPtr = std::shared_ptr<C>;
	}
}
