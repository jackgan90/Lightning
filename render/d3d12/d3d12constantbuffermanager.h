#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "singleton.h"
#include "container.h"
#include "renderconstants.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::container;
		class D3D12ConstantBufferManager : public Foundation::Singleton<D3D12ConstantBufferManager>
		{
			friend class Foundation::Singleton<D3D12ConstantBufferManager>;
		public:
			~D3D12ConstantBufferManager();
			std::size_t AllocBuffer(std::size_t bufferSize);
			std::uint8_t* LockBuffer(std::size_t bufferId);
			//void UnlockBuffer(std::size_t bufferId, container::tuple<std::size_t, std::size_t> dirtyRange);
			D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress(std::size_t bufferId);
			std::size_t GetBufferSize(std::size_t bufferId);
			void ResetBuffers(std::size_t frameIndex);
			void Clear();
		private:
			struct BufferResource
			{
				//resource is allocated and deallocated frequently,use ComPtr has performance hit
				ID3D12Resource* resource;
				std::size_t offset;
				std::size_t size;
				void *mapAddress;
				D3D12_GPU_VIRTUAL_ADDRESS virtualAddress;
			};
			struct BufferAllocation
			{
				BufferResource *resource;
				std::size_t offset;
				std::size_t size;
			};
			D3D12ConstantBufferManager();
			container::list<BufferResource> mBufferResources[RENDER_FRAME_COUNT];
			container::unordered_map<std::size_t, BufferAllocation> mAllocations[RENDER_FRAME_COUNT];
			std::size_t mCurrentID;
			static constexpr std::size_t MIN_BUFFER_SIZE = 2048 * 1024;
			static inline constexpr std::size_t AlignedSize(std::size_t size, std::size_t alignment)
			{
				return (size + (alignment - 1)) & ~(alignment - 1);
			}
		};
	}
}