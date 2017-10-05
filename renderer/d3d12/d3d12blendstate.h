#pragma once
#include <d3d12.h>
#include <vector>
#include <tuple>
#include "iblendstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		class D3D12BlendState : public IBlendState
		{
		public:
			friend class D3D12PipelineStateObject;
			D3D12BlendState();
			const BlendConfiguration* GetConfiguration(const RenderTargetPtr& renderTarget)override;
			bool SetConfiguration(const RenderTargetPtr& renderTarget, const BlendConfiguration& configuration)override;
			bool RemoveConfiguration(const RenderTargetPtr& renderTarget)override;
			bool Enable(const RenderTargetPtr& renderTarget, bool enable)override;
			bool SetFactors(const RenderTargetPtr& renderTarget, BlendFactor srcColor, BlendFactor srcAlpha, BlendFactor destColor, BlendFactor destAlpha)override;
		private:
			struct _PerTargetConfig
			{
				RenderTargetID rtID;
				BlendConfiguration config;
			};
			std::tuple<std::vector<_PerTargetConfig>::iterator, size_t> InternalGetPerTargetConfig(const RenderTargetPtr& renderTarget);
			void UpdateIndependentBlend();
			std::vector<_PerTargetConfig> m_config;
			D3D12_BLEND_DESC m_desc;
		};
	}
}