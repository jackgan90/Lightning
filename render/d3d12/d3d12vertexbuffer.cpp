#include "d3d12vertexbuffer.h"

namespace LightningGE
{
	namespace Render
	{
		//TODO : create ID3D12Resource with D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER will cause 
		//an extra state transition,should refactor later
		D3D12VertexBuffer::D3D12VertexBuffer(ID3D12Device* pDevice, std::uint32_t bufferSize, const std::vector<VertexComponent>& components)
			:VertexBuffer(components), m_resource(pDevice, bufferSize, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
		{
			m_bufferView.BufferLocation = m_resource.GetGPUAddress();
			m_bufferView.SizeInBytes = bufferSize;
			m_bufferView.StrideInBytes = VertexBuffer::GetVertexSize();
		}
	}
}