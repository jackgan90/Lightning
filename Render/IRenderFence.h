#pragma once
#include <cstdint>

namespace Lightning
{
	namespace Render
	{
		struct IRenderFence
		{
			virtual ~IRenderFence() = default;
			virtual void SetTargetValue(std::uint64_t value) = 0;
			virtual std::uint64_t GetTargetValue() = 0;
			virtual std::uint64_t GetCompletedValue() = 0;
			virtual void WaitForTarget() = 0;
		};
	}
}