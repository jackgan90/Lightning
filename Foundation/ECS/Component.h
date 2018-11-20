#pragma once
#undef min
#undef max
#include <cstdint>
#include <memory>
#include "rttr/type"

namespace Lightning
{
	namespace Foundation
	{
		struct Component : std::enable_shared_from_this<Component>
		{
			friend class Entity;
			void Remove();
			std::shared_ptr<Entity> GetOwner()const;
		protected:
			std::weak_ptr<Entity> mOwner;
			RTTR_ENABLE()
		};

		using ComponentPtr = std::shared_ptr<Component>;
	}
}
