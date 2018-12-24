#pragma once
#include "IndexBuffer.h"
#include "D3D12Device.h"
#include "D3D12BufferResource.h"
#include "RefObject.h"

namespace Lightning
{
	namespace Render
	{
		//Thread unsafe
		class D3D12IndexBuffer : public IndexBuffer
		{
		public:
			D3D12IndexBuffer(D3D12Device* pDevice, std::uint32_t bufferSize, IndexType type);
			std::uint8_t* INTERFACECALL Lock(std::size_t start, std::size_t size)override { return mResource.Lock(start, size);};
			void INTERFACECALL Unlock(std::size_t start, std::size_t size)override { mResource.Unlock(start, size); };
			void INTERFACECALL Commit() override{ mResource.Commit(); };
			D3D12_INDEX_BUFFER_VIEW GetBufferView() { return mBufferView; }
		private:
			D3D12_INDEX_BUFFER_VIEW mBufferView;
			D3D12BufferResource mResource;
			REF_OBJECT_OVERRIDE(D3D12IndexBuffer)
		};
	}
}
