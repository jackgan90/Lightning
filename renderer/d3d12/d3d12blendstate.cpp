#include <cstring>
#include <cassert>
#include "d3d12blendstate.h"
#include "d3d12typemapper.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12BlendState::D3D12BlendState():BlendState()
		{
			//TODO : AlphaToCoverageEnable should dig into the meaning.just disable it for the time being
			m_desc.AlphaToCoverageEnable = FALSE;
			m_desc.IndependentBlendEnable = FALSE;
		}

		void D3D12BlendState::UpdateIndependentBlend()
		{
			m_desc.IndependentBlendEnable = m_config.size() > 1;
		}

		bool D3D12BlendState::SetConfiguration(const RenderTargetPtr& renderTarget, const BlendConfiguration& configuration)
		{
			if (!BlendState::SetConfiguration(renderTarget, configuration))
				return false;
			int configIndex = m_config[renderTarget->GetID()].index;
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
			auto oldTargetConfig = InternalGetPerTargetConfig(renderTarget);
			size_t oldSize = m_config.size();
			if (!BlendState::RemoveConfiguration(renderTarget))
				return false;
			int idx = std::get<0>(oldTargetConfig)->index;
			if (idx != oldSize - 1)
			{
				std::memmove(&m_desc.RenderTarget[idx], &m_desc.RenderTarget[idx + 1], 
					(oldSize - idx - 1) * sizeof(D3D12_RENDER_TARGET_BLEND_DESC));
			}
			return true;
		}


		bool D3D12BlendState::Enable(const RenderTargetPtr& renderTarget, bool enable)
		{
			if (!BlendState::Enable(renderTarget, enable))
				return false;
			int configIndex = m_config[renderTarget->GetID()].index;
			UpdateIndependentBlend();
			D3D12_RENDER_TARGET_BLEND_DESC desc = m_desc.RenderTarget[configIndex];
			desc.BlendEnable = enable;
			return true;
		}

		bool D3D12BlendState::SetFactors(const RenderTargetPtr& renderTarget, BlendFactor srcColor, BlendFactor srcAlpha, BlendFactor destColor, BlendFactor destAlpha)
		{
			if (!BlendState::SetFactors(renderTarget, srcColor, srcAlpha, destColor, destAlpha))
				return false;
			int configIndex = m_config[renderTarget->GetID()].index;
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