#pragma once
#include <cstdint>

namespace Lightning
{
	namespace Foundation
	{
		using EventType = std::uint64_t;
		struct IEvent
		{
			virtual ~IEvent() = default;
			virtual EventType TypeIdentifier()const = 0;
		};
	}
}