#pragma once
#include<d3d12.h>
#include <wrl\client.h>
#include "ISwapChain.h"
#include "IRenderTarget.h"
#include "RefObject.h"
#include "D3D12DescriptorHeapManager.h"
#include "D3D12StatefulResource.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		//Thread safe
		class D3D12RenderTarget : public IRenderTarget
		{
		public:
			friend class D3D12RenderTargetManager;
			D3D12RenderTarget(const RenderTargetID rtID, const D3D12StatefulResourcePtr& resource, ISwapChain* pSwapChain);
			INTERFACECALL ~D3D12RenderTarget()override;
			RenderTargetID INTERFACECALL GetID() const override { return mID; }
			std::uint32_t INTERFACECALL GetSampleCount()override { return mSampleCount; }
			int INTERFACECALL GetSampleQuality()override { return mSampleQuality; }
			RenderFormat INTERFACECALL GetRenderFormat()const override { return mFormat; }
			const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return mHeap->cpuHandle; }
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()const { return mHeap->gpuHandle; }
			void TransitToRTState(ID3D12GraphicsCommandList* commandList);
			void TransitToPresentState(ID3D12GraphicsCommandList* commandList);
		private:
			D3D12StatefulResourcePtr mResource;
			RenderTargetID mID;
			DescriptorHeap *mHeap;
			std::uint32_t mSampleCount;
			int mSampleQuality;
			RenderFormat mFormat;
			REF_OBJECT_OVERRIDE(D3D12RenderTarget)
		};
	}
}
