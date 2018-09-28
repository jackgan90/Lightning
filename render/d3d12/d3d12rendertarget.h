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
			RenderTargetID GetID() const override { return mID; }
			ComPtr<ID3D12Resource> GetNative()const { return mResource; }
			const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return mHeap->cpuHandle; }
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()const { return mHeap->gpuHandle; }
			std::size_t GetSampleCount()override { return mSampleCount; }
			int GetSampleQuality()override { return mSampleQuality; }
			RenderFormat GetRenderFormat()const override { return mFormat; }
		private:
			ComPtr<ID3D12Resource> mResource;
			bool mIsSwapChainTarget;
			RenderTargetID mID;
			DescriptorHeap *mHeap;
			std::size_t mSampleCount;
			int mSampleQuality;
			RenderFormat mFormat;
		};
	}
}
