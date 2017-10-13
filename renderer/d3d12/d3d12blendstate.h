#pragma once
#include <d3d12.h>
#include <vector>
#include <tuple>
#include "iblendstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		class D3D12BlendState : public BlendState
		{
		public:
			friend class D3D12PipelineStateObject;
			D3D12BlendState();
			bool SetConfiguration(const SharedRenderTargetPtr& renderTarget, const BlendConfiguration& configuration)override;
			bool RemoveConfiguration(const SharedRenderTargetPtr& renderTarget)override;
			bool Enable(const SharedRenderTargetPtr& renderTarget, bool enable)override;
			bool SetFactors(const SharedRenderTargetPtr& renderTarget, BlendFactor srcColor, BlendFactor srcAlpha, BlendFactor destColor, BlendFactor destAlpha)override;
		private:
			void UpdateIndependentBlend();
			D3D12_BLEND_DESC m_desc;
		};
	}
}