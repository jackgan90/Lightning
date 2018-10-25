#pragma once
#include <wrl\client.h>
#include <d3d12.h>
#include <cstdint>
#include <memory>
#include "boost/noncopyable.hpp"
#include "container.h"
#include "threadlocalsingleton.h"
#include "tbb/spin_mutex.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::container;
		enum D3D12_RESOURCE_TRANSITION_HINTS
		{
			D3D12_RESOURCE_TRANSITION_HINT_FUTURE,
			D3D12_RESOURCE_TRANSITION_HINT_PAST
		};

		class D3D12StatefulResource : private boost::noncopyable
		{
		public:
			explicit D3D12StatefulResource(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState);
			//if hint is D3D12_RESOURCE_TRANSITION_HINT_FUTURE,the resource is guarantee to be in newState before
			//next time the user request it.
			//To achieve this goal,the batched resource transition occurs before any other command recorded by
			//a command list is executed
			//if hint is D3D12_RESOURCE_TRANSITION_HINT_PAST,the resource is not guarantee to be in newState
			//before next time the user request it.
			//To achieve this goal,the batched resource transition occurs after all of the commands recorded by
			//a command list are executed
			//in most cases, D3D12_RESOURCE_TRANSITION_HINT_FUTURE is a proper option unless you know a resource
			//state doesn't affect what you are doing.
			void TransitTo(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState,
				D3D12_RESOURCE_TRANSITION_HINTS hint = D3D12_RESOURCE_TRANSITION_HINT_FUTURE);
			void GetLocalStates(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES& firstState, D3D12_RESOURCE_STATES& finalState);
			D3D12_RESOURCE_STATES GetGlobalState()const { return mGlobalState.state; }
			void UpdateGlobalState(D3D12_RESOURCE_STATES state) { mGlobalState.state = state; }
			ID3D12Resource* operator->()const { return mResource.Get();}
			operator ID3D12Resource*()const{return mResource.Get();}
		private:
			struct ResourceState
			{
				ResourceState():lastAccessFrame(0){}
				std::uint64_t lastAccessFrame;
				D3D12_RESOURCE_STATES state;
				//first state after transit from unknown state
				//basically this state is only for local state.Global state doesn't need it
				D3D12_RESOURCE_STATES firstState;
			};
			using CommandListResourceStates = container::unordered_map<ID3D12GraphicsCommandList*, ResourceState>;
			ComPtr<ID3D12Resource> mResource;
			ResourceState mGlobalState;
			CommandListResourceStates mLocalStates;
			tbb::spin_mutex mtxLocalStates;
		};
		using D3D12StatefulResourcePtr = std::shared_ptr<D3D12StatefulResource>;
	}
}