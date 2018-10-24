#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include "tbb/enumerable_thread_specific.h"
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
			//Thread safe
			D3D12ConstantBuffer AllocBuffer(std::size_t bufferSize);
			//Thread unsafe
			void ResetBuffers(std::size_t frameIndex);
			//Thread unsafe
			void Clear();
		private:
			struct BufferResource
			{
				ComPtr<ID3D12Resource> resource;
				std::size_t offset;
				std::size_t size;
				void *mapAddress;
				D3D12_GPU_VIRTUAL_ADDRESS virtualAddress;
			};
			D3D12ConstantBufferManager();
			using ThreadedBufferResources = tbb::enumerable_thread_specific<container::vector<BufferResource>>;
			ThreadedBufferResources mBufferResources[RENDER_FRAME_COUNT];
			static constexpr std::size_t MIN_BUFFER_SIZE = 2048 * 128;
			static inline constexpr std::size_t AlignedSize(std::size_t size, std::size_t alignment)
			{
				return (size + (alignment - 1)) & ~(alignment - 1);
			}
		};
	}
}