#pragma once
#include <cstdint>

namespace LightningGE
{
	namespace Render
	{
		class IRenderFence
		{
		public:
			virtual ~IRenderFence(){}
			virtual void SetTargetValue(std::uint64_t value) = 0;
			virtual std::uint64_t GetTargetValue() = 0;
			virtual std::uint64_t GetCurrentValue() = 0;
			virtual void WaitForTarget() = 0;
		};
	}
}