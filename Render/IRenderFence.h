#pragma once
#include <cstdint>
#include "Portable.h"

namespace Lightning
{
	namespace Render
	{
		struct IRenderFence
		{
			virtual INTERFACECALL ~IRenderFence() = default;
			virtual void INTERFACECALL SetTargetValue(std::uint64_t value) = 0;
			virtual std::uint64_t INTERFACECALL GetTargetValue() = 0;
			virtual std::uint64_t INTERFACECALL GetCompletedValue() = 0;
			virtual void INTERFACECALL WaitForTarget() = 0;
		};
	}
}