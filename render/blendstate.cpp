#include <boost/functional/hash.hpp>
#include <vector>
#include "iblendstate.h"

namespace Lightning
{
	namespace Renderer
	{
		BlendState::BlendState():m_lastIndex(-1)
		{

		}

		const BlendConfiguration* BlendState::GetConfiguration(const SharedRenderTargetPtr& renderTarget)
		{
			auto perTargetConf = InternalGetPerTargetConfig(renderTarget);
			BlendState::_PerTargetConfig* pConfig = std::get<0>(perTargetConf);
			return pConfig ? &pConfig->config : nullptr;
		}

		BlendState::InternalConfigResult BlendState::InternalGetPerTargetConfig(const SharedRenderTargetPtr& renderTarget)
		{
			if (renderTarget)
			{
				auto it = m_config.find(renderTarget->GetID());
				if (it == m_config.end())
					return std::make_tuple(nullptr, it);
				return std::make_tuple(&it->second, it);
			}
			return std::make_tuple(nullptr, m_config.end());
		}

		bool BlendState::SetConfiguration(const SharedRenderTargetPtr& renderTarget, const BlendConfiguration& configuration)
		{
			if (!renderTarget)
				return false;
			auto perTargetConfig = InternalGetPerTargetConfig(renderTarget);
			BlendState::_PerTargetConfig* pConfig = std::get<0>(perTargetConfig);
			if (pConfig)
			{
				pConfig->config = configuration;
			}
			else
			{
				++m_lastIndex;
				RenderTargetID rtID = renderTarget->GetID();
				m_config.insert(std::make_pair(rtID, _PerTargetConfig{ renderTarget, configuration, m_lastIndex }));
			}
			SetHashDirty();
			return true;
		}

		bool BlendState::RemoveConfiguration(const SharedRenderTargetPtr& renderTarget)
		{
			if (renderTarget)
			{
				auto perTargetConfig = InternalGetPerTargetConfig(renderTarget);
				BlendState::_PerTargetConfig* pConfig = std::get<0>(perTargetConfig);
				if (pConfig)
				{
					auto foundIt = std::get<1>(perTargetConfig);
					for (auto it = m_config.begin(); it != m_config.end(); ++it)
					{
						if (it->second.index > foundIt->second.index)
							--it->second.index;
					}
					m_config.erase(std::get<1>(perTargetConfig));
					--m_lastIndex;
					SetHashDirty();
					return true;
				}
			}
			return false;
		}

		bool BlendState::Enable(const SharedRenderTargetPtr& renderTarget, bool enable)
		{
			if (!renderTarget)
				return false;
			auto perTargetConfig = InternalGetPerTargetConfig(renderTarget);
			BlendState::_PerTargetConfig* pConfig = std::get<0>(perTargetConfig);
			if (pConfig)
			{
				if (pConfig->config.enable != enable)
					SetHashDirty();
				pConfig->config.enable = enable;
			}
			else
			{
				BlendConfiguration configuration;
				configuration.enable = enable;
				++m_lastIndex;
				RenderTargetID rtID = renderTarget->GetID();
				m_config.insert(std::make_pair(rtID, _PerTargetConfig{ renderTarget, configuration, m_lastIndex }));
				SetHashDirty();
			}
			return true;
		}

		bool BlendState::SetFactors(const SharedRenderTargetPtr& renderTarget, BlendFactor srcColor, BlendFactor srcAlpha, BlendFactor destColor, BlendFactor destAlpha)
		{
			if (!renderTarget)
				return false;
			auto perTargetConfig = InternalGetPerTargetConfig(renderTarget);
			BlendState::_PerTargetConfig* pConfig = std::get<0>(perTargetConfig);
			if (pConfig)
			{
				bool changed = pConfig->config.srcColorFactor != srcColor
					|| pConfig->config.srcAlphaFactor != srcAlpha
					|| pConfig->config.destColorFactor != destColor
					|| pConfig->config.destAlphaFactor != destAlpha;
				pConfig->config.srcColorFactor = srcColor;
				pConfig->config.srcAlphaFactor = srcAlpha;
				pConfig->config.destColorFactor = destColor;
				pConfig->config.destAlphaFactor = destAlpha;
				if (changed)
					SetHashDirty();

			}
			else
			{
				BlendConfiguration configuration;
				configuration.srcColorFactor = srcColor;
				configuration.srcAlphaFactor = srcAlpha;
				configuration.destColorFactor = destColor;
				configuration.destAlphaFactor = destAlpha;
				++m_lastIndex;
				RenderTargetID rtID = renderTarget->GetID();
				m_config.insert(std::make_pair(rtID, _PerTargetConfig{ renderTarget, configuration, m_lastIndex }));
				SetHashDirty();
			}
			return true;
		}

		size_t BlendState::CalculateHashInternal()
		{
			size_t seed = 0;
			for (auto it = m_config.begin(); it != m_config.end(); ++it)
			{
				const BlendConfiguration* configuration = GetConfiguration(it->second.renderTarget);
				boost::hash_combine(seed, configuration->alphaOp);
				boost::hash_combine(seed, configuration->colorOp);
				boost::hash_combine(seed, configuration->destAlphaFactor);
				boost::hash_combine(seed, configuration->destColorFactor);
				boost::hash_combine(seed, configuration->enable);
				boost::hash_combine(seed, configuration->srcAlphaFactor);
				boost::hash_combine(seed, configuration->srcColorFactor);
			}
			return seed;
		}

	}
}
