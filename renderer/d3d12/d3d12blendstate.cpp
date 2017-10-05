#include <cstring>
#include <cassert>
#include "d3d12blendstate.h"
#include "d3d12typemapper.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12BlendState::D3D12BlendState()
		{
			//TODO : AlphaToCoverageEnable should dig into the meaning.just disable it for the time being
			m_desc.AlphaToCoverageEnable = FALSE;
			m_desc.IndependentBlendEnable = FALSE;
		}

		std::tuple<std::vector<D3D12BlendState::_PerTargetConfig>::iterator, size_t> D3D12BlendState::InternalGetPerTargetConfig(const RenderTargetPtr& renderTarget)
		{
			if (renderTarget)
			{
				RenderTargetID rtID = renderTarget->GetID();
				size_t idx = 0;
				for (auto it = m_config.begin();it != m_config.end();++it,++idx)
				{
					if (it->rtID == rtID)
					{
						return std::make_tuple(it, idx);
					}
				}
			}
			return std::make_tuple(m_config.end(), -1);
		}

		void D3D12BlendState::UpdateIndependentBlend()
		{
			m_desc.IndependentBlendEnable = m_config.size() > 1;
		}



		const BlendConfiguration* D3D12BlendState::GetConfiguration(const RenderTargetPtr& renderTarget)
		{
			auto result = InternalGetPerTargetConfig(renderTarget);
			auto configIterator = std::get<0>(result);
			if (configIterator == m_config.end())
				return nullptr;
			return &configIterator->config;
		}

		bool D3D12BlendState::SetConfiguration(const RenderTargetPtr& renderTarget, const BlendConfiguration& configuration)
		{
			if (!renderTarget)
				return false;
			int configIndex = 0;
			auto perTargetConfig = InternalGetPerTargetConfig(renderTarget);
			auto configIterator = std::get<0>(perTargetConfig);
			if (configIterator != m_config.end())
			{
				configIterator->config = configuration;
				configIndex = std::get<1>(perTargetConfig);
			}
			else
			{
				m_config.push_back({ renderTarget->GetID(), configuration });
				configIndex = m_config.size() - 1;
			}

			UpdateIndependentBlend();
			D3D12_RENDER_TARGET_BLEND_DESC desc = m_desc.RenderTarget[configIndex];
			desc.BlendEnable = configuration.enable;
			desc.BlendOp = D3D12TypeMapper::MapBlendOp(configuration.colorOp);
			desc.BlendOpAlpha = D3D12TypeMapper::MapBlendOp(configuration.alphaOp);
			desc.SrcBlend = D3D12TypeMapper::MapBlendFactor(configuration.srcColorFactor);
			desc.SrcBlendAlpha = D3D12TypeMapper::MapBlendFactor(configuration.srcAlphaFactor);
			desc.DestBlend = D3D12TypeMapper::MapBlendFactor(configuration.destColorFactor);
			desc.DestBlendAlpha = D3D12TypeMapper::MapBlendFactor(configuration.destAlphaFactor);
			return true;
		}

		bool D3D12BlendState::RemoveConfiguration(const RenderTargetPtr& renderTarget)
		{
			if (renderTarget)
			{
				auto perTargetConfig = InternalGetPerTargetConfig(renderTarget);
				auto configIterator = std::get<0>(perTargetConfig);
				if (configIterator != m_config.end())
				{
					size_t oldSize = m_config.size();
					size_t idx = std::get<1>(perTargetConfig);
					if (idx != m_config.size() - 1)
					{
						std::memmove(&m_desc.RenderTarget[idx], &m_desc.RenderTarget[idx + 1], 
							(oldSize - idx - 1) * sizeof(D3D12_RENDER_TARGET_BLEND_DESC));
					}
					m_config.erase(configIterator);
					return true;
				}
			}
			return false;
		}


		bool D3D12BlendState::Enable(const RenderTargetPtr& renderTarget, bool enable)
		{
			if (!renderTarget)
				return false;
			auto perTargetConfig = InternalGetPerTargetConfig(renderTarget);
			auto configIterator = std::get<0>(perTargetConfig);
			size_t configIndex = 0;
			if (configIterator != m_config.end())
			{
				configIterator->config.enable = enable;
				configIndex = std::get<1>(perTargetConfig);
			}
			else
			{
				BlendConfiguration configuration;
				configuration.enable = enable;
				m_config.push_back({ renderTarget->GetID(), configuration});
				configIndex = m_config.size() - 1;
			}
			UpdateIndependentBlend();
			D3D12_RENDER_TARGET_BLEND_DESC desc = m_desc.RenderTarget[configIndex];
			desc.BlendEnable = enable;
			return true;
		}

		bool D3D12BlendState::SetFactors(const RenderTargetPtr& renderTarget, BlendFactor srcColor, BlendFactor srcAlpha, BlendFactor destColor, BlendFactor destAlpha)
		{
			if (!renderTarget)
				return false;
			auto perTargetConfig = InternalGetPerTargetConfig(renderTarget);
			auto configIterator = std::get<0>(perTargetConfig);
			size_t configIndex = std::get<1>(perTargetConfig);
			if (configIterator != m_config.end())
			{
				configIterator->config.srcColorFactor = srcColor;
				configIterator->config.srcAlphaFactor = srcAlpha;
				configIterator->config.destColorFactor = destColor;
				configIterator->config.destAlphaFactor = destAlpha;
			}
			else
			{
				BlendConfiguration configuration;
				configuration.srcColorFactor = srcColor;
				configuration.srcAlphaFactor = srcAlpha;
				configuration.destColorFactor = destColor;
				configuration.destAlphaFactor = destAlpha;
				m_config.push_back({ renderTarget->GetID(), configuration});
				configIndex = m_config.size() - 1;
			}
			UpdateIndependentBlend();
			D3D12_RENDER_TARGET_BLEND_DESC desc = m_desc.RenderTarget[configIndex];
			desc.SrcBlend = D3D12TypeMapper::MapBlendFactor(srcColor);
			desc.SrcBlendAlpha = D3D12TypeMapper::MapBlendFactor(srcAlpha);
			desc.DestBlend = D3D12TypeMapper::MapBlendFactor(destColor);
			desc.DestBlendAlpha = D3D12TypeMapper::MapBlendFactor(destAlpha);
			return true;
		}

	}
}