#include "common.h"
#include "d3d12pipelinestateobject.h"
#include "d3d12blendstate.h"
#include "d3d12depthstencilstate.h"
#include "d3d12rasterizerstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		bool D3D12PipelineStateObject::ApplyBlendState(const BlendStatePtr& blendState)
		{
			m_blendState = blendState;
			return true;
		}

		bool D3D12PipelineStateObject::ApplyDepthStencilState(const DepthStencilStatePtr& dsState)
		{
			m_depthStencilState = dsState;
			return true;
		}

		bool D3D12PipelineStateObject::ApplyRasterizerState(const RasterizerStatePtr& rasterizerState)
		{
			m_rasterizerState = rasterizerState;
			return true;
		}


		BlendStatePtr D3D12PipelineStateObject::GetBlendState()const
		{
			return m_blendState;
		}

		DepthStencilStatePtr D3D12PipelineStateObject::GetDepthStencilState()const
		{
			return m_depthStencilState;
		}

		RasterizerStatePtr D3D12PipelineStateObject::GetRasterizerState()const
		{
			return m_rasterizerState;
		}

		void D3D12PipelineStateObject::SynchronizeGraphicsDesc()
		{
			m_desc.BlendState = STATIC_CAST_PTR(D3D12BlendState, m_blendState)->m_desc;
			m_desc.DepthStencilState = STATIC_CAST_PTR(D3D12DepthStencilState, m_depthStencilState)->m_desc;
			m_desc.RasterizerState = STATIC_CAST_PTR(D3D12RasterizerState, m_rasterizerState)->m_desc;
		}

	}
}