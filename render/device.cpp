#include <cassert>
#include "device.h"

namespace LightningGE
{
	namespace Render
	{
		Device::Device() :m_devicePipelineState{}
		{

		}

		void Device::ApplyRasterizerState(const RasterizerState& state)
		{
			m_devicePipelineState.rasterizerState = state;
		}

		void Device::ApplyBlendState(const BlendState& state)
		{
			m_devicePipelineState.blendState = state;
		}

		void Device::ApplyDepthStencilState(const DepthStencilState& state)
		{
			m_devicePipelineState.depthStencilState = state;
		}

		void Device::ApplyPipelineState(const PipelineState& state)
		{
			m_devicePipelineState = state;
		}
	}
}