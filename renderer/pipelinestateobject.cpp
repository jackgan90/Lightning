#include <boost/functional/hash.hpp>
#include "ipipelinestateobject.h"

namespace LightningGE
{
	namespace Renderer
	{
		bool PipelineStateObject::ApplyBlendState(const BlendStatePtr & blendState)
		{
			auto oldState = m_blendState;
			m_blendState = blendState;
			if ((!oldState && m_blendState) || (oldState && !m_blendState) ||
				(oldState && m_blendState && oldState->GetHashValue() != m_blendState->GetHashValue()))
				SetHashDirty();
			return true;
		}

		bool PipelineStateObject::ApplyDepthStencilState(const DepthStencilStatePtr & dsState)
		{
			auto oldState = m_depthStencilState;
			m_depthStencilState = dsState;
			if ((!oldState && m_depthStencilState) || (oldState && !m_depthStencilState) ||
				(oldState && m_depthStencilState && oldState->GetHashValue() != m_depthStencilState->GetHashValue()))
				SetHashDirty();
			return true;
		}

		bool PipelineStateObject::ApplyRasterizerState(const RasterizerStatePtr & rasterizerState)
		{
			auto oldState = m_rasterizerState;
			m_rasterizerState = rasterizerState;
			if ((!oldState && m_rasterizerState) || (oldState && !m_rasterizerState) ||
				(oldState && m_rasterizerState && oldState->GetHashValue() != m_rasterizerState->GetHashValue()))
				SetHashDirty();
			return true;
		}

		bool PipelineStateObject::ApplyShader(ShaderType shaderType, const ShaderPtr & shader)
		{
			auto oldShaderIt = m_shaders.find(shaderType);
			m_shaders[shaderType] = shader;
			if ((oldShaderIt == m_shaders.end() && shader) || (oldShaderIt != m_shaders.end() && !shader) ||
				(oldShaderIt != m_shaders.end() && shader && oldShaderIt->second->GetHashValue() != shader->GetHashValue()))
				SetHashDirty();
			return true;
		}

		BlendStatePtr PipelineStateObject::GetBlendState() const
		{
			return m_blendState;
		}

		DepthStencilStatePtr PipelineStateObject::GetDepthStencilState() const
		{
			return m_depthStencilState;
		}

		RasterizerStatePtr PipelineStateObject::GetRasterizerState() const
		{
			return m_rasterizerState;
		}

		ShaderPtr PipelineStateObject::GetShader(ShaderType type) const
		{
			auto it = m_shaders.find(type);
			return it == m_shaders.end() ? nullptr : it->second;
		}

		size_t PipelineStateObject::CalculateHashInternal()
		{
			size_t seed = GetInitialHashSeed();
			if(m_blendState)
				boost::hash_combine(seed, m_blendState->GetHashValue());
			if(m_depthStencilState)
				boost::hash_combine(seed, m_depthStencilState->GetHashValue());
			if(m_rasterizerState)
				boost::hash_combine(seed, m_rasterizerState->GetHashValue());
			for (auto it = m_shaders.begin(); it != m_shaders.end(); ++it)
			{
				if (it->second)
					boost::hash_combine(seed, it->second->GetHashValue());
			}

			return seed;
		}

		size_t PipelineStateObject::GetInitialHashSeed()
		{
			size_t seed = 0;
			//first according to the existence of each state calculate an initial hash value
			if (m_blendState)
				seed |= 1;
			if (m_depthStencilState)
				seed |= 1 << 1;
			if (m_rasterizerState)
				seed |= 1 << 2;
			int shaderShiftBase = 3;
			for (auto it = m_shaders.begin(); it != m_shaders.end(); ++it)
			{
				seed |= 1 << (shaderShiftBase + it->first);
			}
			
			return seed;
		}

	}
}
