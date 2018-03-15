#include "d3d12vertexbuffer.h"

namespace LightningGE
{
	namespace Render
	{
		D3D12VertexBuffer::D3D12VertexBuffer(ID3D12Device* pDevice, std::uint32_t bufferSize, const std::vector<VertexComponent>& components)
			:VertexBuffer(components), m_resource(pDevice, bufferSize)
		{
			m_bufferView.BufferLocation = m_resource.GetGPUAddress();
			m_bufferView.SizeInBytes = bufferSize;
			m_bufferView.StrideInBytes = VertexBuffer::GetVertexSize();
		}
	}
}