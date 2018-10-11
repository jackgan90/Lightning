#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include "singleton.h"
#include "container.h"
#include "renderconstants.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::container;
		struct D3D12ConstantBuffer
		{
			std::uint8_t *userMemory;
			std::size_t size;
			D3D12_GPU_VIRTUAL_ADDRESS virtualAdress;
		};

		class D3D12ConstantBufferManager : public Foundation::Singleton<D3D12ConstantBufferManager>
		{
			friend class Foundation::Singleton<D3D12ConstantBufferManager>;
		public:
			~D3D12ConstantBufferManager();
			D3D12ConstantBuffer AllocBuffer(std::size_t bufferSize);
			//void UnlockBuffer(std::size_t bufferId, container::tuple<std::size_t, std::size_t> dirtyRange);
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
			D3D12ConstantBufferManager();
			container::list<BufferResource> mBufferResources[RENDER_FRAME_COUNT];
			std::size_t mCurrentID;
			static constexpr std::size_t MIN_BUFFER_SIZE = 2048 * 1024;
			static inline constexpr std::size_t AlignedSize(std::size_t size, std::size_t alignment)
			{
				return (size + (alignment - 1)) & ~(alignment - 1);
			}
		};
	}
}