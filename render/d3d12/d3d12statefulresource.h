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
		class D3D12StatefulResource : private boost::noncopyable
		{
		public:
			explicit D3D12StatefulResource(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState);
			void TransitTo(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState);
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
			using CommandListResourceStates = container::unordered_map<ID3D12CommandList*, ResourceState>;
			ComPtr<ID3D12Resource> mResource;
			ResourceState mGlobalState;
			CommandListResourceStates mLocalStates;
			tbb::spin_mutex mtxLocalStates;
		};
		using D3D12StatefulResourcePtr = std::shared_ptr<D3D12StatefulResource>;
	}
}