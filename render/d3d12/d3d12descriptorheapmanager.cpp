#include <iterator>
#include "d3d12descriptorheapmanager.h"
#include "renderer.h"
#include "logger.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::container;
		D3D12DescriptorHeapManager::D3D12DescriptorHeapManager()
		{

		}

		D3D12DescriptorHeapManager::~D3D12DescriptorHeapManager()
		{
			LOG_INFO("Descriptor heap manager destruct!");
#ifndef NDEBUG
			assert(mAllocHeaps.size() == 0 && "D3D12DescriptorHeapManager is destroyed, yet there's still\
				heaps in used which will cause this pointer dangling.It's dangerous.");
#endif
		}

		ID3D12Device* D3D12DescriptorHeapManager::GetNativeDevice()
		{
			return static_cast<D3D12Device*>(Renderer::Instance()->GetDevice())->GetNative();
		}

		DescriptorHeap* D3D12DescriptorHeapManager::Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count, bool frameTransient)
		{
			auto nativeDevice = GetNativeDevice();
			auto typeHash = HeapTypeHash(type, shaderVisible);
			auto it = mHeaps.find(typeHash);
			if (it == mHeaps.end())
			{
				CreateHeapStore(type, shaderVisible, count > HEAP_DESCRIPTOR_ALLOC_SIZE ? count : HEAP_DESCRIPTOR_ALLOC_SIZE, nativeDevice);
				it = mHeaps.find(typeHash);
			}
			auto res = TryAllocateDescriptorHeap(it->second, count, frameTransient);
			if (std::get<0>(res))
				return std::get<1>(res);
			//if reach here,the existing heaps can not allocate more descriptors, so just allocate a new heap
			CreateHeapStore(type, shaderVisible, count > HEAP_DESCRIPTOR_ALLOC_SIZE ? count : HEAP_DESCRIPTOR_ALLOC_SIZE, nativeDevice);
			it = mHeaps.find(typeHash);
			res = TryAllocateDescriptorHeap(it->second, count, frameTransient);
			return std::get<1>(res);
		}

		container::tuple<bool, D3D12DescriptorHeapManager::DescriptorHeapStore*> D3D12DescriptorHeapManager::CreateHeapStore(
			D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT descriptorCount, ID3D12Device* pDevice)
		{
			auto res = std::make_tuple(false, new DescriptorHeapStore);
			auto& heapStore = std::get<1>(res);
			heapStore->desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			heapStore->desc.NodeMask = 0;
			heapStore->desc.NumDescriptors = descriptorCount;
			heapStore->desc.Type = type;
			auto hr = pDevice->CreateDescriptorHeap(&heapStore->desc, IID_PPV_ARGS(&heapStore->heap));
			if (FAILED(hr))
			{
				LOG_ERROR("Failed to create d3d12 descriptor heap!type:%d, flags:%d, number of descriptors:%d", 
					heapStore->desc.Type, heapStore->desc.Flags, heapStore->desc.NumDescriptors);
				return res;
			}
			heapStore->freeDescriptors = descriptorCount;
			heapStore->freeIntervals.emplace_back(std::make_tuple(0, descriptorCount));
			heapStore->cpuHandle = heapStore->heap->GetCPUDescriptorHandleForHeapStart();
			heapStore->gpuHandle = heapStore->heap->GetGPUDescriptorHandleForHeapStart();
			heapStore->incrementSize = GetIncrementSize(type);
			
			auto typeHash = HeapTypeHash(type, shaderVisible);
			if (mHeaps.find(typeHash) == mHeaps.end())
			{
				mHeaps.insert(std::make_pair(typeHash, container::vector<DescriptorHeapStore*>()));
			}
			mHeaps[typeHash].push_back(heapStore);
			std::get<0>(res) =  true;
			return res;
		}

		container::tuple<bool, DescriptorHeap*> D3D12DescriptorHeapManager::TryAllocateDescriptorHeap(container::vector<DescriptorHeapStore*>& heapList, UINT count, bool transient)
		{
			//loop over existing heap list reversely and try to allocate from it
			for (int i = heapList.size() - 1; i >= 0;i--)
			{
				auto res = TryAllocateDescriptorHeap(heapList[i], count, transient);
				if (std::get<0>(res))
					return res;
			}
			return std::make_tuple<bool, DescriptorHeap*>(false, nullptr);
		}

		container::tuple<bool, DescriptorHeap*> D3D12DescriptorHeapManager::TryAllocateDescriptorHeap(DescriptorHeapStore* heapStore, UINT count, bool transient)
		{
			auto res = std::make_tuple<bool, DescriptorHeap*>(false, nullptr);
			if(count > heapStore->freeDescriptors)
				return res;
			for (auto it = heapStore->freeIntervals.begin(); it != heapStore->freeIntervals.end();++it)
			{
				auto left = std::get<0>(*it);
				auto right = std::get<1>(*it);
				auto descriptorCount = right - left;
				if (descriptorCount >= count)
				{
					container::tuple<UINT, UINT> interval;
					//split this interval into 2 intervals and mark left interval as used
					if (descriptorCount == count)
					{
						//no other space for a descriptor, just remove this interval
						heapStore->freeIntervals.erase(it);
						std::get<0>(interval) = left;
						std::get<1>(interval) = right;
					}
					else
					{
						//there's still some space for at least a descriptor,change the interval end points
						*it = std::make_tuple(left + count, right);
						std::get<0>(interval) = left;
						std::get<1>(interval) = left + count;
					}
					heapStore->freeDescriptors -= count;
					std::get<0>(res) = true;
					auto pHeapEx = new DescriptorHeapEx;
					std::get<1>(res) = pHeapEx;
					CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(heapStore->cpuHandle);
					CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(heapStore->gpuHandle);
					cpuHandle.Offset(left * heapStore->incrementSize);
					gpuHandle.Offset(left * heapStore->incrementSize);
					pHeapEx->cpuHandle = cpuHandle;
					pHeapEx->gpuHandle = gpuHandle;
					pHeapEx->incrementSize = heapStore->incrementSize;
					//pHeapEx->offsetInDescriptors = std::get<0>(interval);
					pHeapEx->interval = interval;
					pHeapEx->pStore = heapStore;
					if (transient)
					{
						auto frameIndex = Renderer::Instance()->GetFrameResourceIndex();
						if (mFrameTransientHeaps.find(frameIndex) == mFrameTransientHeaps.end())
						{
							mFrameTransientHeaps.emplace(std::piecewise_construct, std::make_tuple(frameIndex), std::make_tuple());
						}
						mFrameTransientHeaps[frameIndex].push_back(pHeapEx);
					}
#ifndef NDEBUG
					mAllocHeaps.emplace(pHeapEx);
#endif
					break;
				}
			}
			return res;
		}

		void D3D12DescriptorHeapManager::Deallocate(DescriptorHeap* pHeap)
		{
			assert(pHeap != nullptr && "pHeap must be a valid heap pointer!");
#ifndef NDEBUG
			auto it = mAllocHeaps.find(static_cast<DescriptorHeapEx*>(pHeap));
			if (it == mAllocHeaps.end())
			{
				LOG_ERROR("pHeap is not allocated by this manager!");
				return;
			}
			Deallocate(*it);
			mAllocHeaps.erase(it);
#else
			Deallocate(static_cast<DescriptorHeapEx*>(pHeap));
#endif
			delete pHeap;
		}

		void D3D12DescriptorHeapManager::Deallocate(DescriptorHeapEx *pHeapEx)
		{
			auto pHeapStore = pHeapEx->pStore;
			pHeapStore->freeDescriptors += std::get<1>(pHeapEx->interval) - std::get<0>(pHeapEx->interval);
			if (pHeapStore->freeIntervals.empty())
				pHeapStore->freeIntervals.push_back(pHeapEx->interval);
			else
			{
				//insert new interval to appropriate position.If adjacent intervals can be joined,then join them
				auto prevIt = pHeapStore->freeIntervals.end();
				auto nextIt = pHeapStore->freeIntervals.end();
				//first loop intervals and find a gap that can be fit the new interval
				for (auto currIt = pHeapStore->freeIntervals.begin();currIt != pHeapStore->freeIntervals.end();++currIt)
				{
					if(std::get<0>(*currIt) >= std::get<1>(pHeapEx->interval))
					{
						nextIt = currIt;
						break;
					}
					prevIt = currIt;
				}
				if (nextIt == pHeapStore->freeIntervals.end())//no nextIt means append to back
				{
					if (prevIt == pHeapStore->freeIntervals.end())//prevIt also end, which means there's no free interval
					{
						pHeapStore->freeIntervals.push_back(pHeapEx->interval);
					}
					else
					{
						//try to join previous interval with the new interval
						if (std::get<1>(*prevIt) == std::get<0>(pHeapEx->interval))
						{
							std::get<1>(*prevIt) = std::get<1>(pHeapEx->interval);	//joinable
						}
						else
						{
							auto backIt = std::back_inserter(pHeapStore->freeIntervals);
							*backIt = pHeapEx->interval;
						}
					}
				}
				else	//insert between two adjacent intervals.Try to merge if possible
				{
					auto currIt = pHeapStore->freeIntervals.insert(nextIt, pHeapEx->interval);
					if (prevIt != pHeapStore->freeIntervals.end() && std::get<1>(*prevIt) == std::get<0>(pHeapEx->interval))
					{
						std::get<1>(*prevIt) = std::get<1>(pHeapEx->interval);
						pHeapStore->freeIntervals.erase(currIt);
						currIt = prevIt;
					}
					//here must recalculate nextIt,because it may be invalidated by previous code
					auto oldCurrIt = currIt;
					auto nextIt = ++currIt;
					if (std::get<1>(*oldCurrIt) == std::get<0>(*nextIt))
					{
						std::get<1>(*oldCurrIt) = std::get<1>(*nextIt);
						pHeapStore->freeIntervals.erase(nextIt);
					}
				}
			}
		}

		UINT D3D12DescriptorHeapManager::GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type)
		{
			if (mIncrementSizes.find(type) == mIncrementSizes.end())
			{
				auto nativeDevice = GetNativeDevice();
				mIncrementSizes[type] = nativeDevice->GetDescriptorHandleIncrementSize(type);
			}
			return mIncrementSizes[type];
		}

		void D3D12DescriptorHeapManager::Clear()
		{
#ifndef NDEBUG
			for (auto it = mFrameTransientHeaps.begin(); it != mFrameTransientHeaps.end();++it)
			{
				for (auto pHeapEx : it->second)
				{
					Deallocate(static_cast<DescriptorHeap*>(pHeapEx));
				}
			}
			mFrameTransientHeaps.clear();
#endif
			for(auto it = mHeaps.begin(); it != mHeaps.end();++it)
			{
				for (auto p : it->second)
					delete p;
			}
			mHeaps.clear();
		}

		ComPtr<ID3D12DescriptorHeap> D3D12DescriptorHeapManager::GetHeap(DescriptorHeap* pHeap)const
		{
#ifndef NDEBUG
			auto it = mAllocHeaps.find(static_cast<DescriptorHeapEx*>(pHeap));
			if (it == mAllocHeaps.end())
			{
				LOG_ERROR("Invalid descriptor heap in GetHeap:%s:%d", __FILE__, __LINE__);
				return ComPtr<ID3D12DescriptorHeap>();
			}
#endif
			auto pHeapEx = static_cast<DescriptorHeapEx*>(pHeap);
			return pHeapEx->pStore->heap;
		}

		void D3D12DescriptorHeapManager::EraseTransientAllocation(std::size_t frameIndex)
		{
			for (auto pHeapEx : mFrameTransientHeaps[frameIndex])
			{
				Deallocate(static_cast<DescriptorHeap*>(pHeapEx));
			}
			mFrameTransientHeaps[frameIndex].clear();
		}
	}
}