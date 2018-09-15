#pragma once
#include<d3d12.h>
#include <wrl\client.h>
#include "iswapchain.h"
#include "irendertarget.h"
#include "d3d12descriptorheapmanager.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNING_RENDER_API D3D12RenderTarget : public IRenderTarget
		{
		public:
			friend class D3D12RenderTargetManager;
			D3D12RenderTarget(const RenderTargetID rtID, const ComPtr<ID3D12Resource>& resource, ID3D12Device* pNativeDevice, ISwapChain* pSwapChain);
			~D3D12RenderTarget()override;
			bool IsSwapChainRenderTarget()const override;
			RenderTargetID GetID() const override { return m_ID; }
			ComPtr<ID3D12Resource> GetNative()const { return m_resource; }
			const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return m_heap.cpuHandle; }
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()const { return m_heap.gpuHandle; }
			std::size_t GetSampleCount()override { return m_sampleCount; }
			int GetSampleQuality()override { return m_sampleQuality; }
			RenderFormat GetRenderFormat()const override { return m_format; }
		private:
			ComPtr<ID3D12Resource> m_resource;
			bool m_isSwapChainTarget;
			RenderTargetID m_ID;
			DescriptorHeap m_heap;
			std::size_t m_sampleCount;
			int m_sampleQuality;
			RenderFormat m_format;
		};
	}
}
