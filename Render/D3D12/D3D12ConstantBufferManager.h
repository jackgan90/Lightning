#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <cstddef>
#include "Singleton.h"
#include "RenderConstants.h"
#include "D3D12StatefulResource.h"
#include "ThreadLocalObject.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		struct D3D12ConstantBuffer
		{
			std::uint8_t *userMemory;
			std::size_t size;
			D3D12_GPU_VIRTUAL_ADDRESS virtualAdress;
		};

		class D3D12ConstantBufferManager : public Foundation::Singleton<D3D12ConstantBufferManager>
		{
		public:
			~D3D12ConstantBufferManager();
			//Thread safe
			D3D12ConstantBuffer AllocBuffer(std::size_t bufferSize);
			//Thread unsafe
			void Clear();
			static inline constexpr std::size_t AlignedSize(std::size_t size, std::size_t alignment = 256)
			{
				return (size + (alignment - 1)) & ~(alignment - 1);
			}
		private:
			friend class Foundation::Singleton<D3D12ConstantBufferManager>;
			struct BufferResource
			{
				D3D12StatefulResourcePtr resource;
				std::size_t offset;
				std::size_t size;
				void *mapAddress;
				D3D12_GPU_VIRTUAL_ADDRESS virtualAddress;
				std::uint64_t frameCount;
			};
			//Thread unsafe
			BufferResource Reserve(std::size_t bufferSize);
			D3D12ConstantBufferManager();
			//BufferResource mBufferResources[RENDER_FRAME_COUNT];
			Foundation::ThreadLocalObject<std::vector<BufferResource>> mBufferResources[RENDER_FRAME_COUNT];
			//minimum buffer size
			static constexpr std::size_t MIN_BUFFER_SIZE{ 2048 * 10};
		};
	}
}