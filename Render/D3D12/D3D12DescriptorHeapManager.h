#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <tuple>
#include <functional>
#include <wrl\client.h>
#include <atomic>
#include "Singleton.h"
#include "D3D12Device.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		struct DescriptorHeap
		{
			D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle;
			D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;
			UINT incrementSize;
			//offset from heap start.Measured in number of descriptors offset,not byte
			//UINT offsetInDescriptors;
		};

		class D3D12DescriptorHeapManager : public Foundation::Singleton<D3D12DescriptorHeapManager>
		{
		public:
			D3D12DescriptorHeapManager();
			~D3D12DescriptorHeapManager();
			void ReserveFrameDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count);
			DescriptorHeap* Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count, bool frameTransient);
			ComPtr<ID3D12DescriptorHeap> GetHeap(DescriptorHeap* pHeap)const;
			void Deallocate(DescriptorHeap* pHeap);
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
				std::list<std::tuple<UINT, UINT>> freeIntervals;
				std::size_t freeDescriptors;
			};
			struct DescriptorHeapEx : DescriptorHeap
			{
				DescriptorHeapStore *pStore;
				std::tuple<UINT, UINT> interval;
			};

			//represents heaps allocated in one frame
			struct FrameHeap
			{
				std::atomic<UINT> offset;
				std::atomic<std::size_t> allocCount;
				UINT descriptorCount;
				DescriptorHeapStore* heapStore;
				DescriptorHeapEx* handles;
			};
			DescriptorHeap* AllocatePersistentHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count);
			DescriptorHeap* AllocateFrameHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count);
			std::tuple<bool, DescriptorHeapStore*> CreateHeapStore(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT descriptorCount);
			std::tuple<bool, DescriptorHeap*> TryAllocatePersistentHeap(DescriptorHeapStore* heapInfo, UINT count);
			void Deallocate(DescriptorHeapEx* pHeapEx);
			std::tuple<bool, DescriptorHeap*> TryAllocatePersistentHeap(std::vector<DescriptorHeapStore*>& heapList, UINT count);

			//persistent heaps are heaps persist longer than one frame.Examples are RTV and DSV heaps
			std::vector<DescriptorHeapStore*> mPersistentHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][2];
			//frame heaps are heaps that only needs validation in one frame.After a frame finished,the content of heaps doesn't matter
			FrameHeap mFrameHeaps[RENDER_FRAME_COUNT][D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][2];

			UINT sIncrementSizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		};
	}
}