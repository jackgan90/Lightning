#pragma once
#include <wrl/client.h>
#include "D3D12Device.h"
#include "IRenderFence.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		//Thread safe
		class D3D12RenderFence : public IRenderFence
		{
		public:
			D3D12RenderFence(D3D12Device* device, std::uint64_t initial_value);
			~D3D12RenderFence()override;
			void SetTargetValue(std::uint64_t value)override;
			std::uint64_t GetTargetValue()override;
			std::uint64_t GetCompletedValue()override;
			void WaitForTarget()override;
		private:
			ComPtr<ID3D12Fence> mFence;
			HANDLE mEvent;
			std::uint64_t mTargetValue;
		};
	}
}