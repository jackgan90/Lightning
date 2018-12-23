#pragma once
#include <cstddef>
#include <cstdint>
#include "IRefObject.h"

namespace Lightning
{
	namespace Loading
	{
		struct ISerializeBuffer : Plugins::IRefObject
		{
			virtual char* INTERFACECALL GetBuffer() = 0;
			virtual std::size_t INTERFACECALL GetBufferSize()const = 0;
		};
	}
}