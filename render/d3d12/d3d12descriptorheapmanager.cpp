#include "d3d12descriptorheapmanager.h"
#include "renderer.h"
#include "logger.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		D3D12DescriptorHeapManager::D3D12DescriptorHeapManager() :m_currentID(0)
		{

		}

		D3D12DescriptorHeapManager::~D3D12DescriptorHeapManager()
		{
			logger.Log(LogLevel::Info, "Descriptor heap manager destruct!");
		}

		ID3D12Device* D3D12DescriptorHeapManager::GetNativeDevice()
		{
			return static_cast<D3D12Device*>(Renderer::Instance()->GetDevice())->GetNative();
		}

		const HeapAllocationInfo* D3D12DescriptorHeapManager::Create(const D3D12_DESCRIPTOR_HEAP_DESC& desc, ID3D12Device* pDevice)
		{
			ComPtr<ID3D12DescriptorHeap> heap;
			auto nativeDevice = pDevice ? pDevice : GetNativeDevice();
			HRESULT hr = nativeDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
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
			info.incrementSize = GetIncrementSize(desc.Type, pDevice);
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

		UINT D3D12DescriptorHeapManager::GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Device* pDevice)
		{
			if (m_incrementSizes.find(type) == m_incrementSizes.end())
			{
				auto nativeDevice = pDevice ? pDevice : GetNativeDevice();
				m_incrementSizes[type] = nativeDevice->GetDescriptorHandleIncrementSize(type);
			}
			return m_incrementSizes[type];
		}

		void D3D12DescriptorHeapManager::Clear()
		{
			m_heaps.clear();
		}

		ID3D12DescriptorHeap* D3D12DescriptorHeapManager::GetHeap(UINT heapID)const
		{
			auto it = m_heaps.find(heapID);
			if (it == m_heaps.end())
			{
				return nullptr;
			}
			return it->second.heap.Get();
		}


	}
}