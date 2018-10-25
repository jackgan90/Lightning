#include "d3d12statefulresourcemgr.h"
#include "tbb/spin_mutex.h"
#include "renderer.h"
#include "d3d12device.h"
#include "framememoryallocator.h"

namespace
{
	static tbb::spin_mutex mtxResources;
}

namespace Lightning
{
	namespace Render
	{
		extern Foundation::FrameMemoryAllocator g_RenderAllocator;
		void D3D12StatefulResourceMgr::Notify(ID3D12GraphicsCommandList* cmdList, D3D12StatefulResource* resource)
		{
			tbb::spin_mutex::scoped_lock lock(mtxResources);
			mCmdListResources[cmdList].insert(resource);
		}

		void D3D12StatefulResourceMgr::FixResourceStates(container::vector<ID3D12CommandList*>& commandLists)
		{
			tbb::spin_mutex::scoped_lock lock(mtxResources);
			std::size_t i{ 0 };
			mEncoderIndex = 0;
			container::vector<D3D12_RESOURCE_BARRIER> barrierDescs;
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
					auto commandList = GetCommandList();
					commandList->ResourceBarrier(barrierDescs.size(), pMem);
					commandLists.insert(commandLists.begin() + i, static_cast<ID3D12CommandList*>(commandList));
					++i;
				}
				++i;
			}
		}

		ID3D12GraphicsCommandList* D3D12StatefulResourceMgr::GetCommandList()
		{
			auto resourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			auto& commandLists = mEncoders[resourceIndex];

			if (mEncoderIndex >= commandLists.size())
			{
				commandLists.emplace_back();
			}
			else
			{
				commandLists[mEncoderIndex].Reset();
			}
			auto& encoder = commandLists[mEncoderIndex++];
			return encoder.GetCommandList();
		}

		void D3D12StatefulResourceMgr::Clear()
		{
			for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				mEncoders[i].clear();
			}
		}

	}
}