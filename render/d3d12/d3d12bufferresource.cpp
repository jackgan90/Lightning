#include <d3dx12.h>
#include "d3d12bufferresource.h"

namespace Lightning
{
	namespace Render
	{
		D3D12BufferResource::D3D12BufferResource(D3D12Device* device, uint32_t bufferSize, GPUBufferType bufferType)
			: mSize(bufferSize), mLockedPtr(nullptr), mDevice(device), mBufferType(bufferType)
		{
			auto initState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			if(bufferType == GPUBufferType::INDEX)
				initState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
			mDirtyRange.Begin = 0;
			mDirtyRange.End = bufferSize;
			auto nativeDevice = device->GetNative();
			nativeDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
						D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
						initState, nullptr, IID_PPV_ARGS(&mResource));
			nativeDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
						D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
						D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mIntermediateRes));
		}

		std::uint8_t* D3D12BufferResource::Lock(std::size_t start, std::size_t size)
		{
			assert(start >= 0  && size <= mSize && start + size <= mSize);
			if (mLockedPtr)
				return nullptr;
			static const D3D12_RANGE readRange{0, 0};
			mIntermediateRes->Map(0, &readRange, &mLockedPtr);
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
			mIntermediateRes->Unmap(0, &dirtyRange);
			mLockedPtr = nullptr;
		}

		void D3D12BufferResource::Commit()
		{
			if (mDirtyRange.Begin >= mDirtyRange.End)
				return;
			auto bufferState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			if(mBufferType == GPUBufferType::INDEX)
				bufferState = D3D12_RESOURCE_STATE_INDEX_BUFFER;

			auto commandList = mDevice->GetGraphicsCommandList();
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mResource.Get(), bufferState, D3D12_RESOURCE_STATE_COPY_DEST));
			
			commandList->CopyBufferRegion(
				mResource.Get(), mDirtyRange.Begin, mIntermediateRes.Get(), mDirtyRange.Begin, mDirtyRange.End - mDirtyRange.Begin);

			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, bufferState));
			mDirtyRange.Begin = mDirtyRange.End = 0;
		}
	}
}
