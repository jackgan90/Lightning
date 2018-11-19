#pragma once
#undef min
#undef max
#include "IEvent.h"

namespace Lightning
{
	namespace Foundation
	{
		struct Event : IEvent
		{
		public:
			Event(std::uint64_t id):mTypeIdentifier(id){}
			std::uint64_t TypeIdentifier()const override { return mTypeIdentifier; }
		private:
			std::uint64_t mTypeIdentifier;
		};
	}
}