#pragma once
#include <memory>
#include "singleton.h"
#include "d3d12statefulresource.h"

namespace Lightning
{
	namespace Render
	{
		class D3D12StatefulResourceMgr : public Foundation::Singleton<D3D12StatefulResourceMgr>
		{
		public:
			void Notify(ID3D12GraphicsCommandList* cmdList, D3D12StatefulResource* resource);
		private:
			friend class Foundation::Singleton<D3D12StatefulResourceMgr>;
			using CommandListResources = container::unordered_set<D3D12StatefulResource*>;
			D3D12StatefulResourceMgr() = default;
			container::unordered_map<ID3D12GraphicsCommandList*, CommandListResources> mCmdListResources;
		};
	}
}