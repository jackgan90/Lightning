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

		class D3D12RenderFence : public IRenderFence
		{
		public:
			D3D12RenderFence(D3D12Device* device, std::uint64_t initial_value);
			~D3D12RenderFence()override;
			void SetTargetValue(std::uint64_t value)override;
			std::uint64_t GetTargetValue()override;
			std::uint64_t GetCurrentValue()override;
			void WaitForTarget()override;
		private:
			D3D12Device *m_device;
			ComPtr<ID3D12Fence> m_fence;
			HANDLE m_event;
			std::uint64_t m_targetValue;
		};
	}
}