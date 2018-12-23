#pragma once
#include <wrl/client.h>
#include "D3D12Device.h"
#include "IRenderFence.h"
#include "RenderException.h"

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
			INTERFACECALL ~D3D12RenderFence()override;
			void INTERFACECALL SetTargetValue(std::uint64_t value)override;
			std::uint64_t INTERFACECALL GetTargetValue()override;
			std::uint64_t INTERFACECALL GetCompletedValue()override;
			void INTERFACECALL WaitForTarget()override;
		private:
			ComPtr<ID3D12Fence> mFence;
			HANDLE mEvent;
			std::uint64_t mTargetValue;
		};
	}
}