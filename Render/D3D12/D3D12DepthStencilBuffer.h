#pragma once
#include<d3d12.h>
#include <wrl\client.h>
#include "RefObject.h"
#include "IDepthStencilBuffer.h"
#include "D3D12DescriptorHeapManager.h"
#include "D3D12StatefulResource.h"
#include "D3D12Texture.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		//Thread unsafe
		class D3D12DepthStencilBuffer : public IDepthStencilBuffer
		{
		public:
			D3D12DepthStencilBuffer(D3D12Texture* texutre);
			INTERFACECALL ~D3D12DepthStencilBuffer()override;
			void INTERFACECALL SetClearValue(float depthValue=1.0f, std::uint32_t stencilValue=0)override;
			float INTERFACECALL GetDepthClearValue()const override { return mTexture->GetDepthClearValue(); }
			std::uint8_t INTERFACECALL GetStencilClearValue()const override { return mTexture->GetStencilClearValue(); }
			ITexture* INTERFACECALL GetTexture()const override { return mTexture; }
			//Resize the depth stencil buffer.Only use to resize built-in depth stencil buffer on window resize.
			void Resize(std::size_t width, std::size_t height);
			D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return mHeap->CPUHandle; }
			void TransitToState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state);
		private:
			void CreateDepthStencilView();
			D3D12Texture* mTexture;
			DescriptorHeap* mHeap;
			REF_OBJECT_OVERRIDE(D3D12DepthStencilBuffer)
		};
	}
}
