#include <cassert>
#include "device.h"

namespace Lightning
{
	namespace Render
	{
		Device::Device() :mDevicePipelineState{}, mFrameResourceIndex(0)
		{

		}

		void Device::ApplyRasterizerState(const RasterizerState& state)
		{
			mDevicePipelineState.rasterizerState = state;
		}

		void Device::ApplyBlendStates(std::uint8_t firstRTIndex, const BlendState* states, const std::uint8_t stateCount)
		{
			std::memcpy(&mDevicePipelineState.blendStates[firstRTIndex], states, stateCount * sizeof(BlendState));
		}

		void Device::ApplyDepthStencilState(const DepthStencilState& state)
		{
			mDevicePipelineState.depthStencilState = state;
		}

		void Device::ApplyPipelineState(const PipelineState& state)
		{
			mDevicePipelineState = state;
		}

		SharedShaderPtr Device::GetDefaultShader(ShaderType type)
		{
			auto it = mDefaultShaders.find(type);
			if (it == mDefaultShaders.end())
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
			mFrameResourceIndex = frameResourceIndex;
		}
	}
}