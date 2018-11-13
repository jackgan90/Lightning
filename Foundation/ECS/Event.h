#pragma once
#undef min
#undef max
#include <cstdint>
#include <functional>
#include "rttr/type"
#include "entity.h"
#include "container.h"

namespace Lightning
{
	namespace Foundation
	{
		using EventSubscriberID = std::uint64_t;

		struct Event
		{
			virtual ~Event() {}
			RTTR_ENABLE()
		};
		using EventSubscriber = std::function<void(const Event&)>;
	}
}