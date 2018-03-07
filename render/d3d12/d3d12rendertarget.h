#pragma once
#include<d3d12.h>
#include <wrl\client.h>
#include "irendertarget.h"

namespace LightningGE
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDER_API D3D12RenderTarget : public IRenderTarget
		{
		public:
			friend class D3D12RenderTargetManager;
			D3D12RenderTarget(const ComPtr<ID3D12Resource>& pRenderTarget, bool isSwapChainTarget, const RenderTargetID& rtID);
			~D3D12RenderTarget()override;
			bool IsSwapChainRenderTarget()const override;
			std::size_t GetBoundIndex()const override { return m_boundIndex; }
			RenderTargetID GetID() const override { return m_ID; }
			ComPtr<ID3D12Resource> GetNative()const { return m_nativeRenderTarget; }
			const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return m_cpuHandle; }
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()const { return m_gpuHandle; }
			std::size_t GetSampleCount()override { return m_sampleCount; }
			int GetSampleQuality()override { return m_sampleQuality; }
			RenderFormat GetRenderFormat()const override { return m_format; }
		private:
			ComPtr<ID3D12Resource> m_nativeRenderTarget;
			bool m_isSwapChainTarget;
			RenderTargetID m_ID;
			int m_boundIndex;
			D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
			D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
			std::size_t m_sampleCount;
			int m_sampleQuality;
			RenderFormat m_format;
		};
	}
}