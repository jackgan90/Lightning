#include "d3d12statefulresourcemgr.h"
#include "tbb/spin_mutex.h"
#include "renderer.h"
#include "d3d12device.h"
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
		extern Foundation::FrameMemoryAllocator g_RenderAllocator;

		D3D12StatefulResourceMgr::D3D12StatefulResourceMgr()
		{
			std::memset(mEncoders, 0, sizeof(mEncoders));
		}

		void D3D12StatefulResourceMgr::Notify(ID3D12GraphicsCommandList* cmdList, D3D12StatefulResource* resource)
		{
			MutexLock lock(mtxResources);
			mCmdListResources[cmdList].insert(resource);
		}

		void D3D12StatefulResourceMgr::FixResourceStates(Container::Vector<ID3D12CommandList*>& commandLists)
		{
			MutexLock lock(mtxResources);
			std::size_t i{ 0 };
			mEncoderIndex = 0;
			Container::Vector<D3D12_RESOURCE_BARRIER> barrierDescs;
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
						barrier.Transition.pResource = *resource;
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

		ID3D12GraphicsCommandList* D3D12StatefulResourceMgr::GetCommandList()
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

		void D3D12StatefulResourceMgr::Clear()
		{
			for (auto pEncoder : mEncoders)
			{
				if (pEncoder)
					delete pEncoder;
			}
		}

	}
}