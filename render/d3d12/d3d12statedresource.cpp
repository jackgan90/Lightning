#include <d3dx12.h>
#include "d3d12statedresource.h"
#include "renderer.h"

namespace Lightning
{
	namespace Render
	{
		D3D12StatedResource::D3D12StatedResource(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState)
			:mResource(resource)
		{
			mGlobalState.state = initialState;
			mGlobalState.lastAccessFrame = Renderer::Instance()->GetCurrentFrameCount();
		}

		void D3D12StatedResource::TransitTo(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState)
		{
			auto currentFrame = Renderer::Instance()->GetCurrentFrameCount();
			auto localStates = *mLocalStates;
			auto& localState = localStates[commandList];
			//for every command list the first access in a frame should treat this resource as if it is in unknown state
			if (localState.lastAccessFrame < currentFrame)
			{
				//record first state after initial transition
				//we will fix the state before the execution of this command list
				localState.firstState = newState;
			}
			else
			{
				//not the first time this command list request transition.Execute the transition right away
				if (localState.state != newState)
				{
					commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mResource.Get(), localState.state, newState));
				}
			}
			localState.lastAccessFrame = currentFrame;
			localState.state = newState;
		}
	}
}