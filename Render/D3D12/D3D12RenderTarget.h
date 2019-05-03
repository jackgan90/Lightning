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
			D3D12RenderTarget(RenderTargetID rtID, const std::shared_ptr<D3D12Texture>& texture);
			~D3D12RenderTarget()override;
			RenderTargetID GetID() const override { return mID; }
			std::shared_ptr<ITexture> GetTexture()const override { return mTexture; };
			const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return mHeap->CPUHandle; }
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()const { return mHeap->GPUHandle; }
			void TransitToRTState(ID3D12GraphicsCommandList* commandList);
			void TransitToPresentState(ID3D12GraphicsCommandList* commandList);
			//Reset the render target to invalid state(Release referenced D3D12 resources so that we can perform swap chain resize)
			void Reset();
			//Reset with an ID3D12Resource
			void Reset(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES state);
		private:
			RenderTargetID mID;
			std::shared_ptr<D3D12Texture> mTexture;
			DescriptorHeap *mHeap;
		};
	}
}
