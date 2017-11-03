#pragma once
#include <memory>
#include "idevice.h"
#include "ishadermanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API Device : public IDevice
		{
		public:
			Device();
			void ApplyRasterizerState(const RasterizerState& state)override;
			void ApplyBlendState(const BlendState& state)override;
			void ApplyDepthStencilState(const DepthStencilState& state)override;
			void ApplyPipelineState(const PipelineState& state)override;
		protected:
			std::unique_ptr<IShaderManager> m_shaderMgr;
			PipelineState m_currentPipelineState;
		};
	}
}