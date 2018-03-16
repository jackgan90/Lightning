#include <cassert>
#include "device.h"

namespace LightningGE
{
	namespace Render
	{
		Device::Device() :m_devicePipelineState{}, m_frameResourceIndex(0)
		{

		}

		void Device::ApplyRasterizerState(const RasterizerState& state)
		{
			m_devicePipelineState.rasterizerState = state;
		}

		void Device::ApplyBlendStates(std::uint8_t firstRTIndex, const BlendState* states, const std::uint8_t stateCount)
		{
			std::memcpy(&m_devicePipelineState.blendStates[firstRTIndex], states, stateCount * sizeof(BlendState));
		}

		void Device::ApplyDepthStencilState(const DepthStencilState& state)
		{
			m_devicePipelineState.depthStencilState = state;
		}

		void Device::ApplyPipelineState(const PipelineState& state)
		{
			m_devicePipelineState = state;
		}

		SharedShaderPtr Device::GetDefaultShader(ShaderType type)
		{
			auto it = m_defaultShaders.find(type);
			if (it == m_defaultShaders.end())
				return SharedShaderPtr();
			return it->second;
		}

		void Device::ApplyViewports(const RectFList& vp)
		{

		}

		void Device::ApplyScissorRects(const RectFList& scissorRects)
		{

		}

		void Device::BeginFrame(const std::size_t frameResourceIndex)
		{
			m_frameResourceIndex = frameResourceIndex;
		}
	}
}