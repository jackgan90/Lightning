#pragma once
#include <memory>
#include "Singleton.h"
#include "Container.h"
#include "RenderConstants.h"
#include "D3D12CommandEncoder.h"
#include "D3D12StatefulResource.h"

namespace Lightning
{
	namespace Render
	{
		class D3D12StatefulResourceManager : public Foundation::Singleton<D3D12StatefulResourceManager>
		{
		public:
			void Notify(ID3D12GraphicsCommandList* cmdList, D3D12StatefulResource* resource);
			void FixResourceStates(Container::Vector<ID3D12CommandList*>& commandLists);
			void Clear();
		private:
			friend class Foundation::Singleton<D3D12StatefulResourceManager>;
			ID3D12GraphicsCommandList* GetCommandList();
			using CommandListResources = Container::UnorderedSet<D3D12StatefulResource*>;
			D3D12StatefulResourceManager();
			Container::UnorderedMap<ID3D12GraphicsCommandList*, CommandListResources> mCmdListResources;
			//command lists that only use to fix resource states.
			D3D12CommandEncoder* mEncoders[RENDER_FRAME_COUNT];
			std::size_t mEncoderIndex;
		};
	}
}