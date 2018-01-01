#include "d3d12descriptorheapmanager.h"
#include "logger.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		D3D12DescriptorHeapManager::D3D12DescriptorHeapManager(const ComPtr<ID3D12Device>& pdevice)
			:m_device(pdevice),m_currentID(0)
		{

		}

		D3D12DescriptorHeapManager::~D3D12DescriptorHeapManager()
		{
			logger.Log(LogLevel::Info, "Descriptor heap manager destruct!");
		}


		const HeapAllocationInfo* D3D12DescriptorHeapManager::Create(const D3D12_DESCRIPTOR_HEAP_DESC& desc)
		{
			ComPtr<ID3D12DescriptorHeap> heap;
			HRESULT hr = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
			if (FAILED(hr))
			{
				logger.Log(LogLevel::Error, "Failed to create d3d12 descriptor heap!type:%d, flags:%d, number of descriptors:%d", 
					desc.Type, desc.Flags, desc.NumDescriptors);
				return nullptr;
			}
			_HeapAllocationInfoInternal info;
			info.desc = desc;
			info.heap = heap;
			info.cpuHeapStart = heap->GetCPUDescriptorHandleForHeapStart();
			info.gpuHeapStart = heap->GetGPUDescriptorHandleForHeapStart();
			info.incrementSize = GetIncrementSize(desc.Type);
			info.heapID = m_currentID++;
			m_heaps[info.heapID] = info;
			return &m_heaps[info.heapID];
		}

		void D3D12DescriptorHeapManager::Destroy(UINT heapID)
		{
			auto it = m_heaps.find(heapID);
			if (it == m_heaps.end())
				return;
			m_heaps.erase(it);
		}

		UINT D3D12DescriptorHeapManager::GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type)
		{
			if (m_incrementSizes.find(type) == m_incrementSizes.end())
			{
				m_incrementSizes[type] = m_device->GetDescriptorHandleIncrementSize(type);
			}
			return m_incrementSizes[type];
		}
	}
}