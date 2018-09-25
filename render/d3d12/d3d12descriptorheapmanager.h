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
			UINT heapID;
		};

		class D3D12DescriptorHeapManager : public Foundation::Singleton<D3D12DescriptorHeapManager>
		{
		public:
			D3D12DescriptorHeapManager();
			~D3D12DescriptorHeapManager();
			const DescriptorHeap Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count, ID3D12Device* pDevice = nullptr);
			ComPtr<ID3D12DescriptorHeap> GetHeap(UINT heapID)const;
			void Deallocate(D3D12_CPU_DESCRIPTOR_HANDLE handle);
			void Deallocate(D3D12_GPU_DESCRIPTOR_HANDLE handle);
			UINT GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Device* pDevice=nullptr);
			void Clear();
		private:
			static constexpr int HEAP_DESCRIPTOR_ALLOC_SIZE = 100;
			struct _DescriptorHeapInternal : DescriptorHeap
			{
				D3D12_DESCRIPTOR_HEAP_DESC desc;
				ComPtr<ID3D12DescriptorHeap> heap;
				container::list<container::tuple<UINT64, UINT64>> freeIntervals;
				std::size_t freeDescriptors;
				container::unordered_map<UINT64, std::size_t> locationToSizes;
			};
			ID3D12Device* GetNativeDevice();
			UINT HeapTypeHash(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible) { return static_cast<UINT>(type) << 1 | static_cast<UINT>(shaderVisible); }
			container::tuple<bool, _DescriptorHeapInternal> CreateHeapInternal(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT descriptorCount, ID3D12Device* pDevice);
			container::tuple<bool, DescriptorHeap> TryAllocateInternal(_DescriptorHeapInternal& heapInfo, UINT count);
			container::tuple<bool, DescriptorHeap> TryAllocateInternal(container::vector<_DescriptorHeapInternal>& heapList, UINT count);
			void Deallocate(_DescriptorHeapInternal& heapInfo, const UINT64 offset);
			container::unordered_map<UINT, container::vector<_DescriptorHeapInternal>> mHeaps;
			container::unordered_map<UINT, ComPtr<ID3D12DescriptorHeap>> mHeapIDToHeaps;
			container::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, UINT> mIncrementSizes;
			container::unordered_map<UINT64, _DescriptorHeapInternal*> mCPUHandles;
			container::unordered_map<UINT64, _DescriptorHeapInternal*> mGPUHandles;
			UINT mCurrentID;
		};
	}
}