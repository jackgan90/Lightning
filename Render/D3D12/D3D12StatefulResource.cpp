#include <d3dx12.h>
#include "D3D12StatefulResource.h"
#include "D3D12StatefulResourceManager.h"
#include "Renderer.h"

namespace Lightning
{
	namespace Render
	{
		D3D12StatefulResource::D3D12StatefulResource(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState)
			:mResource(resource)
		{
			mDesc = mResource->GetDesc();
			mGlobalState.state = initialState;
			mGlobalState.lastAccessFrame = Renderer::Instance()->GetCurrentFrameCount();
		}

		D3D12StatefulResource::~D3D12StatefulResource()
		{
			mResource.Reset();
		}

		void D3D12StatefulResource::TransitTo(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState)
		{
			auto currentFrame = Renderer::Instance()->GetCurrentFrameCount();
			ResourceState *pLocalState{ nullptr };
			{
				MutexLock lock(mtxLocalStates);
				pLocalState = &mLocalStates[commandList];
			}
			//for every command list the first access in a frame should treat this resource as if it is in unknown state
			if (pLocalState->lastAccessFrame < currentFrame)
			{
				//record first state after initial transition
				//we will fix the state before the execution of this command list
				pLocalState->firstState = newState;
				D3D12StatefulResourceManager::Instance()->Notify(commandList, this);
			}
			else
			{
				//not the first time this command list request transition.Execute the transition right away
				if (pLocalState->state != newState)
				{
					commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mResource.Get(), pLocalState->state, newState));
				}
			}
			pLocalState->lastAccessFrame = currentFrame;
			pLocalState->state = newState;
		}

		void D3D12StatefulResource::GetLocalStates(ID3D12GraphicsCommandList* cmdList, 
			D3D12_RESOURCE_STATES& firstState, D3D12_RESOURCE_STATES& finalState)
		{
			MutexLock lock(mtxLocalStates);
			firstState = mLocalStates[cmdList].firstState;
			finalState = mLocalStates[cmdList].state;
		}

		void D3D12StatefulResource::Reset(const D3D12StatefulResourcePtr& other)
		{
			Reset(other->mResource, other->mGlobalState.state);
		}

		void D3D12StatefulResource::Reset(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState)
		{
			if (resource)
			{
				mResource = resource;
				mDesc = mResource->GetDesc();
				MutexLock lock(mtxLocalStates);
				mGlobalState.state = initialState;
				mGlobalState.lastAccessFrame = Renderer::Instance()->GetCurrentFrameCount();
				mLocalStates.clear();
			}
		}

		void D3D12StatefulResource::Reset()
		{
			mResource.Reset();
			MutexLock lock(mtxLocalStates);
			mLocalStates.clear();
		}
	}
}