#pragma once
#include<d3d12.h>
#include <wrl\client.h>
#include "RefObject.h"
#include "IDepthStencilBuffer.h"
#include "D3D12DescriptorHeapManager.h"
#include "D3D12StatefulResource.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		//Thread unsafe
		class D3D12DepthStencilBuffer : public IDepthStencilBuffer
		{
		public:
			D3D12DepthStencilBuffer(const std::uint32_t width, const std::uint32_t height);
			D3D12DepthStencilBuffer(const std::uint32_t width, const std::uint32_t height, RenderFormat format, const float depthClearValue, const std::uint32_t stencilClearValue);
			INTERFACECALL ~D3D12DepthStencilBuffer()override;
			void INTERFACECALL SetClearValue(float depthValue=1.0f, std::uint32_t stencilValue=0)override;
			float INTERFACECALL GetDepthClearValue()const override;
			std::uint32_t INTERFACECALL GetStencilClearValue()const override;
			RenderFormat INTERFACECALL GetRenderFormat()const override;
			std::uint32_t INTERFACECALL GetWidth()const override { return mWidth; }
			std::uint32_t INTERFACECALL GetHeight()const override { return mHeight; }
			D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return mHeap->cpuHandle; }
			void TransitToState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state);
		private:
			void CreateResource();
			D3D12StatefulResourcePtr mResource;
			float mDepthClearValue;
			std::uint32_t mStencilClearValue;
			RenderFormat mFormat;
			std::uint32_t mWidth;
			std::uint32_t mHeight;
			DescriptorHeap* mHeap;
			REF_OBJECT_OVERRIDE(D3D12DepthStencilBuffer)
		};
	}
}
