#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <tuple>
#include <functional>
#include <wrl\client.h>
#include "container.h"
#include "threadlocalsingleton.h"
#include "singleton.h"
#include "d3d12device.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::container;
		struct DescriptorHeap
		{
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
			UINT incrementSize;
			//offset from heap start.Measured in number of descriptors offset,not byte
			//UINT offsetInDescriptors;
		};

		class D3D12DescriptorHeapManager : public Foundation::Singleton<D3D12DescriptorHeapManager>
		{
		public:
			D3D12DescriptorHeapManager();
			~D3D12DescriptorHeapManager();
			DescriptorHeap* Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count, bool frameTransient);
			ComPtr<ID3D12DescriptorHeap> GetHeap(DescriptorHeap* pHeap)const;
			void Deallocate(DescriptorHeap* pHeap);
			//Thread unsafe
			void EraseTransientAllocation(std::size_t frameIndex, std::size_t reservedSize = 1);
			//Thread safe
			UINT GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type);
			//Thread unsafe
			void Clear();
		private:
			static constexpr int HEAP_DESCRIPTOR_ALLOC_SIZE = 100;
			struct DescriptorHeapStore : DescriptorHeap
			{
				D3D12_DESCRIPTOR_HEAP_DESC desc;
				ComPtr<ID3D12DescriptorHeap> heap;
				container::list<container::tuple<UINT, UINT>> freeIntervals;
				std::size_t freeDescriptors;
			};
			struct DescriptorHeapEx : DescriptorHeap
			{
				DescriptorHeapStore *pStore;
				container::tuple<UINT, UINT> interval;
			};

			//represents heaps allocated in one frame
			struct FrameHeaps
			{
				std::size_t next[RENDER_FRAME_COUNT][D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][2];
				container::vector<DescriptorHeapStore*> heaps[RENDER_FRAME_COUNT][D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][2];
				container::vector<DescriptorHeapEx*> allocations[RENDER_FRAME_COUNT][D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][2];
			};
			DescriptorHeap* AllocatePersistentHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count);
			DescriptorHeap* AllocateFrameHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count);
			container::tuple<bool, DescriptorHeapStore*> CreateHeapStore(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT descriptorCount);
			container::tuple<bool, DescriptorHeap*> TryAllocatePersistentHeap(DescriptorHeapStore* heapInfo, UINT count);
			void Deallocate(DescriptorHeapEx* pHeapEx);
			container::tuple<bool, DescriptorHeap*> TryAllocatePersistentHeap(container::vector<DescriptorHeapStore*>& heapList, UINT count);

			//persistent heaps are heaps persist longer than one frame.Examples are RTV and DSV heaps
			container::vector<DescriptorHeapStore*> mPersistentHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][2];
			//frame heaps are heaps that only needs validation in one frame.After a frame finished,the content of heaps doesn't matter
			Foundation::ThreadLocalSingleton<FrameHeaps> mFrameHeaps;

			UINT sIncrementSizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		};
	}
}