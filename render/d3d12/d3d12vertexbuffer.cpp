#include "d3d12vertexbuffer.h"

namespace Lightning
{
	namespace Render
	{
		//TODO : create ID3D12Resource with D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER will cause 
		//an extra state transition,should refactor later
		D3D12VertexBuffer::D3D12VertexBuffer(ID3D12Device* pDevice, std::uint32_t bufferSize, 
			const VertexDescriptor& descriptor)
			:VertexBuffer(bufferSize, descriptor), mResource(pDevice, bufferSize, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
		{
			mBufferView.BufferLocation = mResource.GetGPUAddress();
			mBufferView.SizeInBytes = bufferSize;
			mBufferView.StrideInBytes = VertexBuffer::GetVertexSize();
		}
	}
}