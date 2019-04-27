#include "D3D12StatefulResourceManager.h"
#include "tbb/spin_mutex.h"
#include "Renderer.h"
#include "D3D12Device.h"
#include "FrameMemoryAllocator.h"

namespace
{
	using Mutex = tbb::spin_mutex;
	using MutexLock = Mutex::scoped_lock;
	static Mutex mtxResources;
}

namespace Lightning
{
	namespace Render
	{
		extern FrameMemoryAllocator g_RenderAllocator;

		D3D12StatefulResourceManager::D3D12StatefulResourceManager()
		{
			std::memset(mEncoders, 0, sizeof(mEncoders));
		}

		void D3D12StatefulResourceManager::Notify(ID3D12GraphicsCommandList* cmdList, D3D12StatefulResource* resource)
		{
			MutexLock lock(mtxResources);
			mCmdListResources[cmdList].insert(resource);
		}

		void D3D12StatefulResourceManager::FixResourceStates(std::vector<ID3D12CommandList*>& commandLists)
		{
			MutexLock lock(mtxResources);
			std::size_t i{ 0 };
			mEncoderIndex = 0;
			static std::vector<D3D12_RESOURCE_BARRIER> barrierDescs;
			barrierDescs.clear();
			while (i < commandLists.size())
			{
				barrierDescs.clear();
				auto cmdList = static_cast<ID3D12GraphicsCommandList*>(commandLists[i]);
				for (auto resource : mCmdListResources[cmdList])
				{
					D3D12_RESOURCE_STATES localFirstState, localFinalState;
					resource->GetLocalStates(cmdList, localFirstState, localFinalState);
					auto globalState = resource->GetGlobalState();
					if (localFirstState != globalState)
					{
						D3D12_RESOURCE_BARRIER desc;
						ZeroMemory(&desc, sizeof(desc));
						D3D12_RESOURCE_BARRIER &barrier = desc;
						desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
						desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
						barrier.Transition.pResource = resource->GetResource();
						barrier.Transition.StateBefore = globalState;
						barrier.Transition.StateAfter = localFirstState;
						barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
						barrierDescs.push_back(desc);
					}
					resource->UpdateGlobalState(localFinalState);
				}
				mCmdListResources[cmdList].clear();
				if (!barrierDescs.empty())
				{
					auto pMem = g_RenderAllocator.Allocate<D3D12_RESOURCE_BARRIER>(barrierDescs.size());
					std::memcpy(pMem, &barrierDescs[0], sizeof(D3D12_RESOURCE_BARRIER) * barrierDescs.size());
					ID3D12GraphicsCommandList* commandList{ nullptr };
					if (i == 0)
						commandList = GetCommandList();
					else
						commandList = static_cast<ID3D12GraphicsCommandList*>(commandLists[i - 1]);
					commandList->ResourceBarrier(UINT(barrierDescs.size()), pMem);
					if (i == 0)
					{
						commandLists.insert(commandLists.begin(), commandList);
						++i;
					}
				}
				++i;
			}
		}

		ID3D12GraphicsCommandList* D3D12StatefulResourceManager::GetCommandList()
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			if (mEncoders[resourceIndex])
			{
				mEncoders[resourceIndex]->Reset();
				return mEncoders[resourceIndex]->GetCommandList();
			}
			else
			{
				mEncoders[resourceIndex] = new D3D12CommandEncoder;
				return mEncoders[resourceIndex]->GetCommandList();
			}
		}

		void D3D12StatefulResourceManager::Clear()
		{
			for (auto pEncoder : mEncoders)
			{
				if (pEncoder)
					delete pEncoder;
			}
		}

	}
}