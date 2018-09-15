#include <d3dx12.h>
#include "d3d12depthstencilbuffer.h"
#include "d3d12descriptorheapmanager.h"
#include "d3d12typemapper.h"
#include "renderer.h"

namespace Lightning
{
	namespace Render
	{
		D3D12DepthStencilBuffer::D3D12DepthStencilBuffer(const std::uint32_t width, const std::uint32_t height):
			m_depthClearValue(1.0f), m_stencilClearValue(0), m_format(RenderFormat::D24_S8), m_width(width), m_height(height)
		{
			CreateResource();
		}

		D3D12DepthStencilBuffer::D3D12DepthStencilBuffer(const std::uint32_t width, const std::uint32_t height,
			RenderFormat format, const float depthClearValue, const std::uint32_t stencilClearValue):
			m_depthClearValue(depthClearValue), m_stencilClearValue(stencilClearValue), m_format(format) 
			,m_width(width), m_height(height)
		{
			CreateResource();
		}

		void D3D12DepthStencilBuffer::CreateResource()
		{
			//create heap
			m_heap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, 1);

			auto nativeFormat = D3D12TypeMapper::MapRenderFormat(m_format);
			//create resource
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = nativeFormat;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

			D3D12_CLEAR_VALUE dsClearValue{};
			dsClearValue.Format = nativeFormat;
			dsClearValue.DepthStencil.Depth = m_depthClearValue;
			dsClearValue.DepthStencil.Stencil = m_stencilClearValue;

			auto nativeDevice = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice())->GetNative();
			nativeDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Tex2D(nativeFormat, m_width, m_height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
				D3D12_RESOURCE_STATE_DEPTH_WRITE, &dsClearValue, IID_PPV_ARGS(&m_resource));
			nativeDevice->CreateDepthStencilView(m_resource.Get(), &dsvDesc, m_heap.cpuHandle);
		}

		D3D12DepthStencilBuffer::~D3D12DepthStencilBuffer()
		{
			m_resource.Reset();
			D3D12DescriptorHeapManager::Instance()->Deallocate(m_heap.cpuHandle);
		}

		void D3D12DepthStencilBuffer::SetClearValue(float depthValue, std::uint32_t stencilValue)
		{

		}

		float D3D12DepthStencilBuffer::GetDepthClearValue()const
		{
			return m_depthClearValue;
		}

		std::uint32_t D3D12DepthStencilBuffer::GetStencilClearValue()const
		{
			return m_stencilClearValue;
		}

		RenderFormat D3D12DepthStencilBuffer::GetRenderFormat()const
		{
			return m_format;
		}
	}
}
