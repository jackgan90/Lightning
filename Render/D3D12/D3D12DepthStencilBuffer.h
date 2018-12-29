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
			D3D12DepthStencilBuffer(D3D12Texture* texutre, float depthClearValue = 1.0f, std::uint8_t stencilClearValue = 0);
			INTERFACECALL ~D3D12DepthStencilBuffer()override;
			void INTERFACECALL SetClearValue(float depthValue=1.0f, std::uint32_t stencilValue=0)override;
			float INTERFACECALL GetDepthClearValue()const override;
			std::uint8_t INTERFACECALL GetStencilClearValue()const override;
			ITexture* INTERFACECALL GetTexture()const override { return mTexture; }
			D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return mHeap->cpuHandle; }
			void TransitToState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state);
		private:
			void CreateResource();
			float mDepthClearValue;
			std::uint8_t mStencilClearValue;
			D3D12Texture* mTexture;
			DescriptorHeap* mHeap;
			REF_OBJECT_OVERRIDE(D3D12DepthStencilBuffer)
		};
	}
}
