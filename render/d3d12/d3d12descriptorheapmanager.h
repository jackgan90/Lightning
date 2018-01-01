#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <unordered_map>
#include <vector>
#include <wrl\client.h>
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

		class D3D12DescriptorHeapManager
		{
		public:
			friend class D3D12Renderer;
			D3D12DescriptorHeapManager(const ComPtr<ID3D12Device>& pdevice);
			~D3D12DescriptorHeapManager();
			const HeapAllocationInfo* Create(const D3D12_DESCRIPTOR_HEAP_DESC& desc);
			void Destroy(UINT heapID);
			UINT GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type);
		private:
			struct _HeapAllocationInfoInternal : HeapAllocationInfo
			{
				ComPtr<ID3D12DescriptorHeap> heap;
			};
			ComPtr<ID3D12Device> m_device;
			std::unordered_map<UINT, _HeapAllocationInfoInternal> m_heaps;
			std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, UINT> m_incrementSizes;
			UINT m_currentID;
		};
	}
}