#include <d3dx12.h>
#include "D3D12BufferResource.h"
#include "D3D12Renderer.h"

namespace Lightning
{
	namespace Render
	{
		D3D12BufferResource::D3D12BufferResource(D3D12Device* device, uint32_t bufferSize, GPUBufferType bufferType)
			: mSize(bufferSize), mLockedPtr(nullptr), mBufferType(bufferType)
		{
			auto initState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			if(bufferType == GPUBufferType::INDEX)
				initState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
			mDirtyRange.Begin = 0;
			mDirtyRange.End = bufferSize;
			mResource = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
				initState, nullptr);
			mIntermediateRes = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
		}

		std::uint8_t* D3D12BufferResource::Lock(std::size_t start, std::size_t size)
		{
			assert(start >= 0  && size <= mSize && start + size <= mSize);
			if (mLockedPtr)
				return nullptr;
			static const D3D12_RANGE readRange{0, 0};
			(*mIntermediateRes)->Map(0, &readRange, &mLockedPtr);
			return reinterpret_cast<std::uint8_t*>(mLockedPtr) + start;
		}

		void D3D12BufferResource::Unlock(std::size_t start, std::size_t size)
		{
			if (!mLockedPtr)
				return;
			if (start < mDirtyRange.Begin)
				mDirtyRange.Begin = start;
			auto end = start + size;
			if (end > mDirtyRange.End)
				mDirtyRange.End = end;
			D3D12_RANGE dirtyRange{start, start + size};
			(*mIntermediateRes)->Unmap(0, &dirtyRange);
			mLockedPtr = nullptr;
		}

		void D3D12BufferResource::Commit()
		{
			if (mDirtyRange.Begin >= mDirtyRange.End)
				return;
			auto bufferState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			if(mBufferType == GPUBufferType::INDEX)
				bufferState = D3D12_RESOURCE_STATE_INDEX_BUFFER;

			auto commandList = static_cast<D3D12Renderer*>(Renderer::Instance())->GetGraphicsCommandList();
			mResource->TransitTo(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
			
			commandList->CopyBufferRegion(
				(*mResource), mDirtyRange.Begin, (*mIntermediateRes), mDirtyRange.Begin, mDirtyRange.End - mDirtyRange.Begin);

			mResource->TransitTo(commandList, bufferState);
			mDirtyRange.Begin = mDirtyRange.End = 0;
		}
	}
}
