#pragma once
#include <memory>
#include "singleton.h"
#include "container.h"
#include "renderconstants.h"
#include "d3d12commandencoder.h"
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
			friend class Foundation::Singleton<D3D12StatefulResourceMgr>;
			ID3D12GraphicsCommandList* GetCommandList();
			using CommandListResources = container::unordered_set<D3D12StatefulResource*>;
			D3D12StatefulResourceMgr() = default;
			container::unordered_map<ID3D12GraphicsCommandList*, CommandListResources> mCmdListResources;
			//command lists that only use to fix resource states.
			container::vector<D3D12CommandEncoder> mStateFixCmdLists[RENDER_FRAME_COUNT];
			std::size_t mFixCmdListIndex;
		};
	}
}