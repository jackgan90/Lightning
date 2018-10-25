#pragma once
#include <memory>
#include "singleton.h"
#include "container.h"
#include "d3d12statefulresource.h"

namespace Lightning
{
	namespace Render
	{
		class D3D12StatefulResourceMgr : public Foundation::Singleton<D3D12StatefulResourceMgr>
		{
		public:
			void Notify(ID3D12GraphicsCommandList* cmdList, D3D12StatefulResource* resource);
			void FixResourceStates(container::vector<ID3D12CommandList*>& commandLists);
			void Clear();
		private:
			struct D3D12CommandEncoder
			{
				ComPtr<ID3D12CommandAllocator> commandAllocator;
				ComPtr<ID3D12CommandList> commandList;
			};
			friend class Foundation::Singleton<D3D12StatefulResourceMgr>;
			ID3D12GraphicsCommandList* GetCommandList();
			using CommandListResources = container::unordered_set<D3D12StatefulResource*>;
			D3D12StatefulResourceMgr() : mFixCmdListIndex(0){}
			container::unordered_map<ID3D12GraphicsCommandList*, CommandListResources> mCmdListResources;
			//command lists that only use to fix resource states.
			container::vector<D3D12CommandEncoder> mStateFixCmdLists;
			std::size_t mFixCmdListIndex;
		};
	}
}