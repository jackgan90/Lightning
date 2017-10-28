#include <cassert>
#include "device.h"

namespace LightningGE
{
	namespace Renderer
	{
		Device::Device() :m_currentPipelineState{}
		{

		}

		SharedShaderPtr Device::CreateShader(ShaderType type, const std::string& shaderFileName, const ShaderDefine& defineMap)
		{
			assert(m_shaderMgr != nullptr && "You must create IShaderManager instance in derived class!");
			return m_shaderMgr->GetShader(type, shaderFileName, defineMap);
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