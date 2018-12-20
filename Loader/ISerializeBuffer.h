#pragma once
#include <cstdint>
#include "RefObject.h"

namespace Lightning
{
	namespace Loading
	{
		class ISerializeBuffer : public Plugins::RefObject
		{
		public:
			virtual char* GetBuffer() = 0;
			virtual std::size_t GetBufferSize()const = 0;
		};
	}
}