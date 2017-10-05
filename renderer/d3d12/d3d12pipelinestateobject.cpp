#include "d3d12pipelinestateobject.h"

namespace LightningGE
{
	namespace Renderer
	{
		bool D3D12PipelineStateObject::ApplyBlendState(const BlendStatePtr& blendState)
		{
			return false;
		}

		bool D3D12PipelineStateObject::ApplyDepthStencilState(const DepthStencilStatePtr& dsState)
		{
			return false;
		}

		BlendStatePtr D3D12PipelineStateObject::GetBlendState()const
		{
			return BlendStatePtr();
		}

		DepthStencilStatePtr D3D12PipelineStateObject::GetDepthStencilState()const
		{
			return DepthStencilStatePtr();
		}
	}
}