#pragma once
#include <cstdint>
#include "RefCount.h"

namespace Lightning
{
	namespace Loading
	{
		class ISerializeBuffer : public Plugins::RefCount
		{
		public:
			virtual char* GetBuffer() = 0;
			virtual std::size_t GetBufferSize()const = 0;
		};
	}
}