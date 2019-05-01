#pragma once
#include <cstddef>
#include <cstdint>
#include "IRefObject.h"

namespace Lightning
{
	namespace Loading
	{
		struct ISerializeBuffer
		{
			virtual ~ISerializeBuffer() = default;
			virtual char* GetBuffer() = 0;
			virtual std::size_t GetBufferSize()const = 0;
		};
	}
}