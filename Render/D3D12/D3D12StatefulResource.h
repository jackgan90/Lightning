#pragma once
#include <wrl\client.h>
#include <d3d12.h>
#include <cstdint>
#include <memory>
#include "boost/noncopyable.hpp"
#include "Container.h"
#include "ThreadLocalObject.h"
#include "tbb/spin_mutex.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::Container;
		class D3D12StatefulResource;
		using D3D12StatefulResourcePtr = std::shared_ptr<D3D12StatefulResource>;
		class D3D12StatefulResource : private boost::noncopyable
		{
		public:
			explicit D3D12StatefulResource(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState);
			~D3D12StatefulResource();
			void TransitTo(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState);
			void GetLocalStates(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES& firstState, D3D12_RESOURCE_STATES& finalState);
			D3D12_RESOURCE_STATES GetGlobalState()const { return mGlobalState.state; }
			void UpdateGlobalState(D3D12_RESOURCE_STATES state) { mGlobalState.state = state; }
			//Replace internal resource with the new one.
			void Reset(const D3D12StatefulResourcePtr& other);
			//Same with last one,but accept raw ComPtr<ID3D12Resource>, used in swap chain buffer resize.
			void Reset(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState);
			//Release internal resource reference.Risky!Calling this method will change the state of the object to an
			//invalid state.You should know exactly what you are doing.
			void Reset();
			ID3D12Resource* GetResource()const { return mResource.Get(); }
			const D3D12_RESOURCE_DESC& GetDesc()const { return mDesc; }
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
			using CommandListResourceStates = Container::UnorderedMap<ID3D12GraphicsCommandList*, ResourceState>;
			ComPtr<ID3D12Resource> mResource;
			D3D12_RESOURCE_DESC mDesc;
			ResourceState mGlobalState;
			CommandListResourceStates mLocalStates;
			using Mutex = tbb::spin_mutex;
			using MutexLock = Mutex::scoped_lock;
			Mutex mtxLocalStates;
		};
	}
}