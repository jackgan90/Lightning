#pragma once
#include <wrl\client.h>
#include <d3d12.h>
#include <cstdint>
#include "container.h"
#include "threadlocalsingleton.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::container;
		class D3D12StatedResource
		{
		public:
			D3D12StatedResource(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState);
			void TransitTo(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState);
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
			Foundation::ThreadLocalSingleton<CommandListResourceStates> mLocalStates;
		};
	}
}