#pragma once
#include <d3d12.h>
#include "ipipelinestateobject.h"

namespace LightningGE
{
	namespace Renderer
	{
		class D3D12PipelineStateObject : public IPipelineStateObject
		{
		public:
			bool ApplyBlendState(const BlendStatePtr& blendState)override;
			bool ApplyDepthStencilState(const DepthStencilStatePtr& dsState)override;
			BlendStatePtr GetBlendState()const override;
			DepthStencilStatePtr GetDepthStencilState()const override;
		};
	}
}