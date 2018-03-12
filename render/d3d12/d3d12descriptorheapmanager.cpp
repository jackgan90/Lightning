#include <iterator>
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

		const DescriptorHeap D3D12DescriptorHeapManager::Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count, ID3D12Device* pDevice)
		{
			auto nativeDevice = pDevice ? pDevice : GetNativeDevice();
			auto typeHash = HeapTypeHash(type, shaderVisible);
			auto it = m_heaps.find(typeHash);
			if (it == m_heaps.end())
			{
				CreateHeapInternal(type, shaderVisible, count > HEAP_DESCRIPTOR_ALLOC_SIZE ? count : HEAP_DESCRIPTOR_ALLOC_SIZE, nativeDevice);
				it = m_heaps.find(typeHash);
			}
			auto res = TryAllocateInternal(it->second, count);
			if (std::get<0>(res))
				return std::get<1>(res);
			//if reach here,the existing heaps can not allocate more descriptors, so just allocate a new heap
			auto newHeap = CreateHeapInternal(type, shaderVisible, count > HEAP_DESCRIPTOR_ALLOC_SIZE ? count : HEAP_DESCRIPTOR_ALLOC_SIZE, nativeDevice);
			res = TryAllocateInternal(std::get<1>(newHeap), count);
			return std::get<1>(res);
		}

		std::tuple<bool, D3D12DescriptorHeapManager::_DescriptorHeapInternal> D3D12DescriptorHeapManager::CreateHeapInternal(
			D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT descriptorCount, ID3D12Device* pDevice)
		{
			auto res = std::make_tuple(false, _DescriptorHeapInternal());
			auto& heapInfo = std::get<1>(res);
			heapInfo.desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			heapInfo.desc.NodeMask = 0;
			heapInfo.desc.NumDescriptors = descriptorCount;
			heapInfo.desc.Type = type;
			auto hr = pDevice->CreateDescriptorHeap(&heapInfo.desc, IID_PPV_ARGS(&heapInfo.heap));
			if (FAILED(hr))
			{
				logger.Log(LogLevel::Error, "Failed to create d3d12 descriptor heap!type:%d, flags:%d, number of descriptors:%d", 
					heapInfo.desc.Type, heapInfo.desc.Flags, heapInfo.desc.NumDescriptors);
				return res;
			}
			heapInfo.freeDescriptors = descriptorCount;
			heapInfo.freeIntervals.emplace_back(std::make_tuple(0, descriptorCount));
			heapInfo.cpuHandle = heapInfo.heap->GetCPUDescriptorHandleForHeapStart();
			heapInfo.gpuHandle = heapInfo.heap->GetGPUDescriptorHandleForHeapStart();
			heapInfo.incrementSize = GetIncrementSize(type);
			heapInfo.heapID = m_currentID;
			
			auto typeHash = HeapTypeHash(type, shaderVisible);
			if (m_heaps.find(typeHash) == m_heaps.end())
			{
				m_heaps.insert(std::make_pair(typeHash, std::vector<_DescriptorHeapInternal>()));
			}
			m_heaps[typeHash].push_back(heapInfo);
			m_heapIDToHeaps[m_currentID] = heapInfo.heap;
			m_currentID++;
			std::get<0>(res) =  true;
			return res;
		}

		std::tuple<bool, DescriptorHeap> D3D12DescriptorHeapManager::TryAllocateInternal(std::vector<_DescriptorHeapInternal>& heapList, UINT count)
		{
			//loop over existing heap list reversely and try to allocate from it
			for (int i = heapList.size() - 1; i >= 0;i--)
			{
				auto allocateResult = TryAllocateInternal(heapList[i], count);
				if (std::get<0>(allocateResult))
					return allocateResult;
			}
			return std::make_tuple(false, DescriptorHeap());
		}

		std::tuple<bool, DescriptorHeap> D3D12DescriptorHeapManager::TryAllocateInternal(_DescriptorHeapInternal& heapInfo, UINT count)
		{
			auto res = std::make_tuple(false, DescriptorHeap());
			if(count > heapInfo.freeDescriptors)
				return res;
			for (auto it = heapInfo.freeIntervals.begin(); it != heapInfo.freeIntervals.end();++it)
			{
				auto leftEndPoint = std::get<0>(*it);
				auto rightEndPoint = std::get<1>(*it);
				auto descriptorsInInterval = rightEndPoint - leftEndPoint;
				if (descriptorsInInterval >= count)
				{
					//split this interval into 2 intervals and mark left interval as used
					if (descriptorsInInterval == count)
					{
						//no other space for a descriptor, just remove this interval
						heapInfo.freeIntervals.erase(it);
					}
					else
					{
						//there's still some space for at least a descriptor,change the interval end points
						auto rightInterval = std::make_tuple(leftEndPoint + count, rightEndPoint);
						*it = rightInterval;
					}
					heapInfo.freeDescriptors -= count;
					std::get<0>(res) = true;
					auto& info = std::get<1>(res);
					CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(heapInfo.cpuHandle);
					CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(heapInfo.gpuHandle);
					cpuHandle.Offset(leftEndPoint * heapInfo.incrementSize);
					gpuHandle.Offset(leftEndPoint * heapInfo.incrementSize);
					m_cpuHandleToInternal[cpuHandle.ptr] = &heapInfo;
					m_gpuHandleToInternal[gpuHandle.ptr] = &heapInfo;
					heapInfo.locationToSizes[leftEndPoint] = count;
					info.heapID = heapInfo.heapID;
					info.cpuHandle = cpuHandle;
					info.gpuHandle = gpuHandle;
					break;
				}
			}
			return res;
		}

		void D3D12DescriptorHeapManager::Deallocate(D3D12_CPU_DESCRIPTOR_HANDLE handle)
		{
			auto it = m_cpuHandleToInternal.find(handle.ptr);
			assert(it != m_cpuHandleToInternal.end());
			//calculate handle offset in heap
			std::size_t offset = (handle.ptr - it->second->cpuHandle.ptr) / it->second->incrementSize;
			Deallocate(*it->second, offset);
			//erase gpu handle as well
			m_gpuHandleToInternal.erase(it->second->gpuHandle.ptr + offset * it->second->incrementSize);
			m_cpuHandleToInternal.erase(it);
		}

		void D3D12DescriptorHeapManager::Deallocate(D3D12_GPU_DESCRIPTOR_HANDLE handle)
		{
			auto it = m_gpuHandleToInternal.find(handle.ptr);
			assert(it != m_gpuHandleToInternal.end());
			std::size_t offset = (handle.ptr - it->second->cpuHandle.ptr) / it->second->incrementSize;
			Deallocate(*it->second, offset);
			//erase cpu handle as well
			m_cpuHandleToInternal.erase(it->second->cpuHandle.ptr + offset * it->second->incrementSize);
			m_gpuHandleToInternal.erase(it);
		}

		void D3D12DescriptorHeapManager::Deallocate(_DescriptorHeapInternal& heapInfo, const std::size_t offset)
		{
			auto it = heapInfo.locationToSizes.find(offset);
			assert(it != heapInfo.locationToSizes.end());
			auto newInterval = std::make_tuple(offset, offset + it->second);
			heapInfo.freeDescriptors += it->second;
			if (heapInfo.freeIntervals.empty())
				heapInfo.freeIntervals.push_back(newInterval);
			else
			{
				//insert new interval to appropriate position.If adjacent intervals can be joined,then join them
				auto prevIt = heapInfo.freeIntervals.end();
				auto nextIt = heapInfo.freeIntervals.end();
				//first loop intervals and find a gap that can be fit the new interval
				for (auto currIt = heapInfo.freeIntervals.begin();currIt != heapInfo.freeIntervals.end();++currIt)
				{
					if(std::get<0>(*currIt) >= offset + it->second)
					{
						nextIt = currIt;
						break;
					}
					prevIt = currIt;
				}
				if (nextIt == heapInfo.freeIntervals.end())//no nextIt means append to back
				{
					//try to join previous interval with the new interval
					if (std::get<1>(*prevIt) == offset)
					{
						std::get<1>(*prevIt) = offset + it->second;	//joinable
					}
					else
					{
						auto backIt = std::back_inserter(heapInfo.freeIntervals);
						*backIt = newInterval;
					}
				}
				else
				{
					auto currIt = heapInfo.freeIntervals.insert(nextIt, newInterval);
					if (prevIt != heapInfo.freeIntervals.end() && std::get<1>(*prevIt) == offset)
					{
						std::get<1>(*prevIt) = offset + it->second;
						heapInfo.freeIntervals.erase(currIt);
						currIt = prevIt;
					}

					if (std::get<0>(*nextIt) == offset + it->second)
					{
						std::get<1>(*currIt) = std::get<1>(*nextIt);
						heapInfo.freeIntervals.erase(nextIt);
					}
				}
			}
			heapInfo.locationToSizes.erase(it);
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
			m_heapIDToHeaps.clear();
			m_cpuHandleToInternal.clear();
			m_gpuHandleToInternal.clear();
		}

		ComPtr<ID3D12DescriptorHeap> D3D12DescriptorHeapManager::GetHeap(UINT heapID)const
		{
			auto it = m_heapIDToHeaps.find(heapID);
			if (it == m_heapIDToHeaps.end())
			{
				return ComPtr<ID3D12DescriptorHeap>();
			}
			return it->second;
		}


	}
}