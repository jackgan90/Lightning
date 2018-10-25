#pragma once
#include<d3d12.h>
#include <wrl\client.h>
#include "iswapchain.h"
#include "irendertarget.h"
#include "d3d12descriptorheapmanager.h"
#include "d3d12statefulresource.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		//Thread safe
		class LIGHTNING_RENDER_API D3D12RenderTarget : public IRenderTarget
		{
		public:
			friend class D3D12RenderTargetManager;
			D3D12RenderTarget(const RenderTargetID rtID, const D3D12StatefulResourcePtr& resource, ISwapChain* pSwapChain);
			~D3D12RenderTarget()override;
			bool IsSwapChainRenderTarget()const override;
			RenderTargetID GetID() const override { return mID; }
			ID3D12Resource* GetNative()const { return (*mResource); }
			const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return mHeap->cpuHandle; }
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()const { return mHeap->gpuHandle; }
			std::size_t GetSampleCount()override { return mSampleCount; }
			int GetSampleQuality()override { return mSampleQuality; }
			RenderFormat GetRenderFormat()const override { return mFormat; }
		private:
			D3D12StatefulResourcePtr mResource;
			bool mIsSwapChainTarget;
			RenderTargetID mID;
			DescriptorHeap *mHeap;
			std::size_t mSampleCount;
			int mSampleQuality;
			RenderFormat mFormat;
		};
	}
}
