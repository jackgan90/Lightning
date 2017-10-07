#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <unordered_map>
#include <vector>
#include <wrl\client.h>
#include "d3d12device.h"
#include "irenderresourcekeeper.h"

namespace LightningGE
{
	namespace Renderer
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

		class D3D12DescriptorHeapManager : public IRenderResourceKeeper
		{
		public:
			friend class D3D12RenderContext;
			static D3D12DescriptorHeapManager* Instance();
			const HeapAllocationInfo* Create(const D3D12_DESCRIPTOR_HEAP_DESC& desc);
			void Destroy(UINT heapID);
			UINT GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type);
			void ReleaseRenderResources()override;
		private:
			struct _HeapAllocationInfoInternal : HeapAllocationInfo
			{
				ComPtr<ID3D12DescriptorHeap> heap;
			};
			static D3D12DescriptorHeapManager* CreateInstance(ComPtr<ID3D12Device> pdevice);
			ComPtr<ID3D12Device> m_device;
			D3D12DescriptorHeapManager(ComPtr<ID3D12Device> pdevice);
			std::unordered_map<UINT, _HeapAllocationInfoInternal> m_heaps;
			std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, UINT> m_incrementSizes;
			UINT m_currentID;
			static D3D12DescriptorHeapManager* s_instance;
		};
	}
}