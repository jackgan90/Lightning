#pragma once
#include <d3d12.h>
#include "ipipelinestateobject.h"

namespace LightningGE
{
	namespace Renderer
	{
		class D3D12PipelineStateObject : public PipelineStateObject
		{
		public:
			friend class D3D12Renderer;
			void ReleaseRenderResources()override;
		private:
			void SynchronizeGraphicsDesc();
			D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc;
		};
	}
}