#pragma once
#include <cstddef>
#include <cstdint>
#include "IRefObject.h"

namespace Lightning
{
	namespace Loading
	{
		class ISerializeBuffer : public Plugins::IRefObject
		{
		public:
			virtual char* GetBuffer() = 0;
			virtual std::size_t GetBufferSize()const = 0;
		};
	}
}