#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <unordered_map>
#include <vector>
#include <wrl\client.h>
#include "singleton.h"
#include "d3d12device.h"

namespace LightningGE
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		const unsigned AUTO_ALLOCATED_SIZE = 3;
		struct HeapAllocationInfo
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHeapStart;
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHeapStart;
			UINT incrementSize;
			UINT heapID;
		};

		class D3D12DescriptorHeapManager : public Foundation::Singleton<D3D12DescriptorHeapManager>
		{
		public:
			D3D12DescriptorHeapManager();
			~D3D12DescriptorHeapManager();
			const HeapAllocationInfo* Create(const D3D12_DESCRIPTOR_HEAP_DESC& desc, ID3D12Device* pDevice=nullptr);
			ID3D12DescriptorHeap* GetHeap(UINT heapID)const;
			void Destroy(UINT heapID);
			UINT GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Device* pDevice=nullptr);
			void Clear();
		private:
			ID3D12Device* GetNativeDevice();
			struct _HeapAllocationInfoInternal : HeapAllocationInfo
			{
				ComPtr<ID3D12DescriptorHeap> heap;
			};
			std::unordered_map<UINT, _HeapAllocationInfoInternal> m_heaps;
			std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, UINT> m_incrementSizes;
			UINT m_currentID;
		};
	}
}