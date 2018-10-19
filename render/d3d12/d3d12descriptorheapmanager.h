#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <tuple>
#include <functional>
#include <wrl\client.h>
#include "container.h"
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
			void EraseTransientAllocation(std::size_t frameIndex);
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
			ID3D12Device* GetNativeDevice();
			container::tuple<bool, DescriptorHeapStore*> CreateHeapStore(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT descriptorCount, ID3D12Device* pDevice);
			container::tuple<bool, DescriptorHeap*> TryAllocateDescriptorHeap(DescriptorHeapStore* heapInfo, UINT count, bool transient);
			void Deallocate(DescriptorHeapEx* pHeapEx);
			static std::size_t HeapIndex(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible);
#ifdef LIGHTNING_RENDER_MT
			container::tuple<bool, DescriptorHeap*> TryAllocateDescriptorHeap(container::concurrent_vector<DescriptorHeapStore*>& heapList, UINT count, bool transient);
			container::concurrent_vector<DescriptorHeapEx*> mFrameTransientHeaps[RENDER_FRAME_COUNT];
			container::concurrent_vector<DescriptorHeapStore*>* mHeaps;
#else
			container::tuple<bool, DescriptorHeap*> TryAllocateDescriptorHeap(container::vector<DescriptorHeapStore*>& heapList, UINT count, bool transient);
			container::vector<DescriptorHeapEx*> mFrameTransientHeaps[RENDER_FRAME_COUNT];
			container::vector<DescriptorHeapStore*>* mHeaps;
#endif
			UINT sIncrementSizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
#ifndef NDEBUG
			container::unordered_set<DescriptorHeapEx*> mAllocHeaps;
#endif
		};
	}
}