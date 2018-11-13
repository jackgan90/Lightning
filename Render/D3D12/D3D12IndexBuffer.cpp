#include <D3D12TypeMapper.h>
#include "D3D12IndexBuffer.h"

namespace Lightning
{
	namespace Render
	{
		D3D12IndexBuffer::D3D12IndexBuffer(D3D12Device* pDevice, std::uint32_t bufferSize, IndexType type):
			IndexBuffer(bufferSize, type), mResource(pDevice, bufferSize, GPUBufferType::INDEX)
		{
			mBufferView.BufferLocation = mResource.GetGPUVirtualAddress();
			mBufferView.SizeInBytes = bufferSize;
			mBufferView.Format = D3D12TypeMapper::MapIndexType(type);
		}
	}
}