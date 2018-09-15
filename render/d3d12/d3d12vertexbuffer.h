#pragma once
#include "vertexbuffer.h"
#include "d3d12bufferresource.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API D3D12VertexBuffer : public VertexBuffer
		{
		public:
			D3D12VertexBuffer(ID3D12Device* pDevice, std::uint32_t bufferSize, const std::vector<VertexComponent>& components);
			D3D12_VERTEX_BUFFER_VIEW GetBufferView() { return m_bufferView; }
			ComPtr<ID3D12Resource> GetResource() { return m_resource.GetDefaultHeap(); }
			ComPtr<ID3D12Resource> GetIntermediateResource() { return m_resource.GetUploadHeap(); }
		private:
			D3D12BufferResource m_resource;
			D3D12_VERTEX_BUFFER_VIEW m_bufferView;
		};
	}
}
