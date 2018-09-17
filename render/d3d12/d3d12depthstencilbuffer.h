#pragma once
#include<d3d12.h>
#include <wrl\client.h>
#include "idepthstencilbuffer.h"
#include "d3d12descriptorheapmanager.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNING_RENDER_API D3D12DepthStencilBuffer : public IDepthStencilBuffer
		{
		public:
			D3D12DepthStencilBuffer(const std::uint32_t width, const std::uint32_t height);
			D3D12DepthStencilBuffer(const std::uint32_t width, const std::uint32_t height, RenderFormat format, const float depthClearValue, const std::uint32_t stencilClearValue);
			~D3D12DepthStencilBuffer()override;
			void SetClearValue(float depthValue=1.0f, std::uint32_t stencilValue=0)override;
			float GetDepthClearValue()const override;
			std::uint32_t GetStencilClearValue()const override;
			RenderFormat GetRenderFormat()const override;
			D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return mHeap.cpuHandle; }
			std::uint32_t GetWidth()const override { return mWidth; }
			std::uint32_t GetHeight()const override { return mHeight; }
		private:
			void CreateResource();
			ComPtr<ID3D12Resource> mResource;
			float mDepthClearValue;
			std::uint32_t mStencilClearValue;
			RenderFormat mFormat;
			std::uint32_t mWidth;
			std::uint32_t mHeight;
			DescriptorHeap mHeap;
		};
	}
}
