#include <d3d12typemapper.h>
#include "d3d12indexbuffer.h"

namespace LightningGE
{
	namespace Render
	{
		D3D12IndexBuffer::D3D12IndexBuffer(ID3D12Device* pDevice, std::uint32_t bufferSize, IndexType type):
			IndexBuffer(type), m_resource(pDevice, bufferSize, D3D12_RESOURCE_STATE_INDEX_BUFFER)
		{
			m_bufferView.BufferLocation = m_resource.GetGPUAddress();
			m_bufferView.SizeInBytes = bufferSize;
			m_bufferView.Format = D3D12TypeMapper::MapIndexType(type);
		}
	}
}