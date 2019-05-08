#include "tbb/mutex.h"
#include "D3D12DescriptorHeapManager.h"
#include "Renderer.h"
#include "Logger.h"

namespace {
	using Mutex = tbb::mutex;
	using MutexLock = Mutex::scoped_lock;
	static Mutex mtxHeap;
}

namespace Lightning
{
	namespace Render
	{
		UINT D3D12DescriptorHeapManager::sIncrementSizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		D3D12DescriptorHeapManager::D3D12DescriptorHeapManager()
		{
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			for (auto i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;++i)
			{
				sIncrementSizes[i] = device->GetDescriptorHandleIncrementSize(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
			}
		}

		D3D12DescriptorHeapManager::~D3D12DescriptorHeapManager()
		{

		}


		DescriptorHeap* D3D12DescriptorHeapManager::Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count, bool transient)
		{
			if (transient)
			{
				return AllocateTransientHeap(type, shaderVisible, count);
			}
			else
			{
				return AllocatePersistentHeap(type, shaderVisible, count);
			}
		}

		D3D12DescriptorHeapManager::TransientHeap D3D12DescriptorHeapManager::ReserveTransientDescriptors(
			D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count, std::uint64_t frameCount)
		{
			assert(count > 0 && "descriptor count must be positive!");
			TransientHeap transientHeap;
			count = std::max(count, TransientHeapAllocationUnit);
			transientHeap.heapStore = CreateHeapStore(type, shaderVisible, count);
			transientHeap.handles = new DescriptorHeapAllocation[count];
			transientHeap.descriptorCount = count;
			transientHeap.allocCount = 0;
			transientHeap.frameCount = frameCount;
			return transientHeap;
		}

		DescriptorHeap* D3D12DescriptorHeapManager::AllocateTransientHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count)
		{
			assert(count > 0);
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto frameCount = Renderer::Instance()->GetCurrentFrameCount();
			auto i = shaderVisible ? 1 : 0;
			auto& transientHeapList = mTransientHeaps[resourceIndex][type][i].Local();
			if (transientHeapList.empty() || transientHeapList.back().frameCount != frameCount)
			{
				if (transientHeapList.size() == 1 && transientHeapList.back().descriptorCount >= count)
				{
					transientHeapList.back().allocCount = 0;
					transientHeapList.back().frameCount = frameCount;
				}
				else
				{
					UINT totalDescriptorCount{ 0 };
					std::for_each(transientHeapList.begin(), transientHeapList.end(), 
						[&totalDescriptorCount](TransientHeap& heap) {
						totalDescriptorCount += heap.descriptorCount;
						delete heap.heapStore;
						delete[] heap.handles;
					});
					transientHeapList.clear();
					transientHeapList.emplace_back(ReserveTransientDescriptors(type, shaderVisible, std::max(count, totalDescriptorCount), frameCount));
				}
			}
			
			if (transientHeapList.back().allocCount + count > transientHeapList.back().descriptorCount)
			{
				transientHeapList.emplace_back(ReserveTransientDescriptors(type, shaderVisible, count, frameCount));
			}
			auto& transientHeap = transientHeapList.back();
			CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle(transientHeap.heapStore->CPUHandle);
			CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle(transientHeap.heapStore->GPUHandle);
			CPUHandle.Offset(transientHeap.allocCount * transientHeap.heapStore->incrementSize);
			GPUHandle.Offset(transientHeap.allocCount * transientHeap.heapStore->incrementSize);
			
			auto heap = &transientHeap.handles[transientHeap.allocCount];
			transientHeap.allocCount += count;
			heap->CPUHandle = CPUHandle;
			heap->GPUHandle = GPUHandle;
			heap->incrementSize = transientHeap.heapStore->incrementSize;
			heap->pStore = transientHeap.heapStore;
			return heap;
		}

		DescriptorHeap* D3D12DescriptorHeapManager::AllocatePersistentHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT count)
		{
			MutexLock lock(mtxHeap);
			auto i = shaderVisible ? 1 : 0;
			if (mPersistentHeaps[type][i].empty())
			{
				auto store = CreateHeapStore(type, shaderVisible, count > HEAP_DESCRIPTOR_ALLOC_SIZE ? count : HEAP_DESCRIPTOR_ALLOC_SIZE);
				mPersistentHeaps[type][i].push_back(store);
			}
			auto heapAllocation = TryAllocatePersistentHeap(mPersistentHeaps[type][i], count);
			if (heapAllocation)
				return heapAllocation;
			//if reach here,the existing heaps can not allocate more descriptors, so just allocate a new heap
			auto store = CreateHeapStore(type, shaderVisible, count > HEAP_DESCRIPTOR_ALLOC_SIZE ? count : HEAP_DESCRIPTOR_ALLOC_SIZE);
			mPersistentHeaps[type][i].push_back(store);
			return TryAllocatePersistentHeap(mPersistentHeaps[type][i], count);
		}

		D3D12DescriptorHeapManager::DescriptorHeapStore* D3D12DescriptorHeapManager::CreateHeapStore(
			D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT descriptorCount)
		{
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			auto heapStore = new DescriptorHeapStore;
			heapStore->desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			heapStore->desc.NodeMask = 0;
			heapStore->desc.NumDescriptors = descriptorCount;
			heapStore->desc.Type = type;
			heapStore->heap = device->CreateDescriptorHeap(&heapStore->desc);
			if (!heapStore->heap)
			{
				LOG_ERROR("Failed to create d3d12 descriptor heap!type:%d, flags:%d, number of descriptors:%d", 
					heapStore->desc.Type, heapStore->desc.Flags, heapStore->desc.NumDescriptors);
				delete heapStore;
				return nullptr;
			}
			heapStore->freeDescriptors = descriptorCount;
			heapStore->freeIntervals.emplace_back(std::make_tuple(0, descriptorCount));
			heapStore->CPUHandle = heapStore->heap->GetCPUDescriptorHandleForHeapStart();
			heapStore->GPUHandle = heapStore->heap->GetGPUDescriptorHandleForHeapStart();
			heapStore->incrementSize = GetIncrementSize(type);
			
			return heapStore;
		}

		DescriptorHeap* D3D12DescriptorHeapManager::TryAllocatePersistentHeap(std::vector<DescriptorHeapStore*>& heapList, UINT count)
		{
			//loop over existing heap list reversely and try to allocate from it
			for (long long i = heapList.size() - 1; i >= 0;i--)
			{
				auto res = TryAllocatePersistentHeap(heapList[i], count);
				if (res)
					return res;
			}
			return nullptr;
		}

		DescriptorHeap* D3D12DescriptorHeapManager::TryAllocatePersistentHeap(DescriptorHeapStore* heapStore, UINT count)
		{
			if(count > heapStore->freeDescriptors)
				return nullptr;
			for (auto it = heapStore->freeIntervals.begin(); it != heapStore->freeIntervals.end();++it)
			{
				auto left = std::get<0>(*it);
				auto right = std::get<1>(*it);
				auto descriptorCount = right - left;
				if (descriptorCount >= count)
				{
					std::tuple<UINT, UINT> interval;
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
					auto heapAllocation = new DescriptorHeapAllocation;
					CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle(heapStore->CPUHandle);
					CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle(heapStore->GPUHandle);
					CPUHandle.Offset(left * heapStore->incrementSize);
					GPUHandle.Offset(left * heapStore->incrementSize);
					heapAllocation->CPUHandle = CPUHandle;
					heapAllocation->GPUHandle = GPUHandle;
					heapAllocation->incrementSize = heapStore->incrementSize;
					heapAllocation->interval = interval;
					heapAllocation->pStore = heapStore;
					return heapAllocation;
				}
			}
			return nullptr;
		}

		void D3D12DescriptorHeapManager::Deallocate(DescriptorHeap* pHeap)
		{
			assert(pHeap != nullptr && "pHeap must be a valid heap pointer!");
			Deallocate(static_cast<DescriptorHeapAllocation*>(pHeap));
			delete pHeap;
		}

		void D3D12DescriptorHeapManager::Deallocate(DescriptorHeapAllocation *heapAllocation)
		{
			auto pHeapStore = heapAllocation->pStore;
			MutexLock lock(mtxHeap);
			pHeapStore->freeDescriptors += std::get<1>(heapAllocation->interval) - std::get<0>(heapAllocation->interval);
			if (pHeapStore->freeIntervals.empty())
				pHeapStore->freeIntervals.push_back(heapAllocation->interval);
			else
			{
				//insert new interval to appropriate position.If adjacent intervals can be joined,then join them
				auto prevIt = pHeapStore->freeIntervals.end();
				auto nextIt = pHeapStore->freeIntervals.end();
				//first loop intervals and find a gap that can be fit the new interval
				for (auto currIt = pHeapStore->freeIntervals.begin();currIt != pHeapStore->freeIntervals.end();++currIt)
				{
					if(std::get<0>(*currIt) >= std::get<1>(heapAllocation->interval))
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
						pHeapStore->freeIntervals.push_back(heapAllocation->interval);
					}
					else
					{
						//try to join previous interval with the new interval
						if (std::get<1>(*prevIt) == std::get<0>(heapAllocation->interval))
						{
							std::get<1>(*prevIt) = std::get<1>(heapAllocation->interval);	//joinable
						}
						else
						{
							pHeapStore->freeIntervals.push_back(heapAllocation->interval);
						}
					}
				}
				else	//insert between two adjacent intervals.Try to merge if possible
				{
					auto currIt = pHeapStore->freeIntervals.insert(nextIt, heapAllocation->interval);
					if (prevIt != pHeapStore->freeIntervals.end() && std::get<1>(*prevIt) == std::get<0>(heapAllocation->interval))
					{
						std::get<1>(*prevIt) = std::get<1>(heapAllocation->interval);
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
			return sIncrementSizes[type];
		}

		void D3D12DescriptorHeapManager::Clear()
		{
				//TODO : clear frame transient heap
			for (auto i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
			{
				for (auto j = 0;j < RENDER_FRAME_COUNT;++j)
				{
					for (auto k = 0; k < 2; ++k)
					{
						mTransientHeaps[j][i][k].for_each([](TransientHeapList& heapList) {
							for (auto heap : heapList)
							{
								delete heap.heapStore;
								delete[] heap.handles;
							}
						});
					}
				}
				for (auto j = 0;j < 2;++j)
				{
					for (auto p : mPersistentHeaps[i][j])
					{
						delete p;
					}
					mPersistentHeaps[i][j].clear();
				}
			}
		}

		ComPtr<ID3D12DescriptorHeap> D3D12DescriptorHeapManager::GetHeap(DescriptorHeap* pHeap)const
		{
			auto heapAllocation = static_cast<DescriptorHeapAllocation*>(pHeap);
			return heapAllocation->pStore->heap;
		}
	}
}