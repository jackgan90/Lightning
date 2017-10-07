#include "common.h"
#include "d3d12pipelinestateobject.h"
#include "d3d12blendstate.h"
#include "d3d12depthstencilstate.h"
#include "d3d12rasterizerstate.h"

namespace LightningGE
{
	namespace Renderer
	{

		void D3D12PipelineStateObject::SynchronizeGraphicsDesc()
		{
			m_desc.BlendState = STATIC_CAST_PTR(D3D12BlendState, m_blendState)->m_desc;
			m_desc.DepthStencilState = STATIC_CAST_PTR(D3D12DepthStencilState, m_depthStencilState)->m_desc;
			m_desc.RasterizerState = STATIC_CAST_PTR(D3D12RasterizerState, m_rasterizerState)->m_desc;
		}

		void D3D12PipelineStateObject::ReleaseRenderResources()
		{
			//TODO : release render resources
		}


	}
}