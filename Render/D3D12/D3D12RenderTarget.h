#pragma once
#include<d3d12.h>
#include <wrl\client.h>
#include "ISwapChain.h"
#include "IRenderTarget.h"
#include "RefObject.h"
#include "D3D12DescriptorHeapManager.h"
#include "D3D12Texture.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		//Thread safe
		class D3D12RenderTarget : public IRenderTarget
		{
		public:
			D3D12RenderTarget(RenderTargetID rtID, D3D12Texture* texture);
			INTERFACECALL ~D3D12RenderTarget()override;
			RenderTargetID INTERFACECALL GetID() const override { return mID; }
			ITexture* INTERFACECALL GetTexture()const override { return mTexture; };
			const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return mHeap->cpuHandle; }
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()const { return mHeap->gpuHandle; }
			void TransitToRTState(ID3D12GraphicsCommandList* commandList);
			void TransitToPresentState(ID3D12GraphicsCommandList* commandList);
			//Reset the render target to invalid state(Release referenced D3D12 resources so that we can perform swap chain resize)
			void Reset();
			//Reset with an ID3D12Resource
			void Reset(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES state);
		private:
			RenderTargetID mID;
			D3D12Texture* mTexture;
			DescriptorHeap *mHeap;
			REF_OBJECT_OVERRIDE(D3D12RenderTarget)
		};
	}
}
