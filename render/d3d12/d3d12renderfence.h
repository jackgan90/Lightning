#pragma once
#include <wrl/client.h>
#include "d3d12device.h"
#include "irenderfence.h"
#include "renderexception.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class FenceInitDexception : public RendererException
		{
		public:
			FenceInitDexception(const char* w) : RendererException(w){}
		};
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