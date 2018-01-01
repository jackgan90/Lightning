#include <cassert>
#include "device.h"

namespace LightningGE
{
	namespace Render
	{
		Device::Device() :m_currentPipelineState{}
		{

		}

		void Device::ApplyRasterizerState(const RasterizerState& state)
		{
			m_currentPipelineState.rasterizerState = state;
		}

		void Device::ApplyBlendState(const BlendState& state)
		{
			m_currentPipelineState.blendState = state;
		}

		void Device::ApplyDepthStencilState(const DepthStencilState& state)
		{
			m_currentPipelineState.depthStencilState = state;
		}

		void Device::ApplyPipelineState(const PipelineState& state)
		{
			m_currentPipelineState = state;
		}
	}
}