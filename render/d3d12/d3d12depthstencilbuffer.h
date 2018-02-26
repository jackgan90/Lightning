#pragma once
#include<d3d12.h>
#include <wrl\client.h>
#include "idepthstencilbuffer.h"

namespace LightningGE
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDER_API D3D12DepthStencilBuffer : public IDepthStencilBuffer
		{
		public:
			D3D12DepthStencilBuffer(const std::uint32_t width, const std::uint32_t height);
			D3D12DepthStencilBuffer(const std::uint32_t width, const std::uint32_t height, RenderFormat format, const float depthClearValue, const std::uint32_t stencilClearValue);
			~D3D12DepthStencilBuffer()override;
			void SetClearValue(float depthValue=1.0f, std::uint32_t stencilValue=0)override;
			float GetDepthClearValue()const override;
			std::uint32_t GetStencilClearValue()const override;
			RenderFormat GetRenderFormat()const override;
			D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()const { return m_cpuHandle; }
			std::uint32_t GetWidth()const override { return m_width; }
			std::uint32_t GetHeight()const override { return m_height; }
		private:
			void CreateResource();
			ComPtr<ID3D12Resource> m_resource;
			float m_depthClearValue;
			std::uint32_t m_stencilClearValue;
			RenderFormat m_format;
			std::uint32_t m_width;
			std::uint32_t m_height;
			D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
			D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
			//not sure if it's efficient to split the heap creation on multiple objects,should profile
			UINT m_heapID;
		};
	}
}