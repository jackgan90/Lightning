#pragma once
#include<d3d12.h>
#include <wrl\client.h>
#include "irendertarget.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12RenderTarget : public IRenderTarget
		{
		public:
			friend class D3D12RenderTargetManager;
			D3D12RenderTarget(ComPtr<ID3D12Resource> pRenderTarget, bool isSwapChainTarget, const RenderTargetID& rtID);
			~D3D12RenderTarget()override;
			bool IsSwapChainRenderTarget()const override;
			RenderTargetID GetID() const override { return m_ID; }
			ComPtr<ID3D12Resource> GetNative()const { return m_nativeRenderTarget; }
			const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return m_cpuHandle; }
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()const { return m_gpuHandle; }
		private:
			ComPtr<ID3D12Resource> m_nativeRenderTarget;
			bool m_isSwapChainTarget;
			RenderTargetID m_ID;
			D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
			D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
		};
	}
}