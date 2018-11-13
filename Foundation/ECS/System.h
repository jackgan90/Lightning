#pragma once
#include <cstdint>
#include <memory>
#include "Entity.h"

namespace Lightning
{
	namespace Foundation
	{
		using SystemPriority = std::uint32_t;
		
		class ISystem
		{
		public:
			virtual ~ISystem() {}
			virtual void Update(const EntityPtr& entity) = 0;
			virtual SystemPriority GetPriority()const = 0;
			static constexpr SystemPriority DefaultPriority = 0xffffffffU;
		};

		class System : public ISystem
		{
		public:
			System(const SystemPriority& priority) :mPriority(priority){}
			SystemPriority GetPriority()const { return mPriority; }
		protected:
			friend class SystemManager;
			SystemPriority mPriority;
		};
		
		template<typename T>
		using SystemPtr = std::shared_ptr<T>;
	}
}