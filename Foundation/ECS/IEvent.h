#pragma once
#include <cstdint>
#include "Portable.h"

namespace Lightning
{
	namespace Foundation
	{
		using EventType = std::uint64_t;
		struct IEvent
		{
			virtual INTERFACECALL ~IEvent() = default;
			virtual EventType INTERFACECALL TypeIdentifier()const = 0;
		};
	}
}