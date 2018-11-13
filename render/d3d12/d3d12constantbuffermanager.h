#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <atomic>
#include "singleton.h"
#include "container.h"
#include "renderconstants.h"
#include "d3d12statefulresource.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::Container;
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
			//Thread unsafe
			void Reserve(std::size_t bufferSize);
			//Thread safe
			D3D12ConstantBuffer AllocBuffer(std::size_t bufferSize);
			//Thread unsafe
			void Clear();
			static inline constexpr std::size_t AlignedSize(std::size_t size, std::size_t alignment = 256)
			{
				return (size + (alignment - 1)) & ~(alignment - 1);
			}
		private:
			struct BufferResource
			{
				D3D12StatefulResourcePtr resource;
				std::atomic<std::size_t> offset;
				std::size_t size;
				void *mapAddress;
				D3D12_GPU_VIRTUAL_ADDRESS virtualAddress;
			};
			D3D12ConstantBufferManager();
			BufferResource mBufferResources[RENDER_FRAME_COUNT];
		};
	}
}