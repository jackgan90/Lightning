#pragma once
#include <memory>
#include "idevice.h"
#include "ishadermanager.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API Device : public IDevice
		{
		public:
			Device();
			void ApplyPipelineState(const PipelineState& state)override;
			SharedShaderPtr GetDefaultShader(ShaderType type)override;
		protected:
			virtual void ApplyRasterizerState(const RasterizerState& state);
			virtual void ApplyBlendStates(const std::uint8_t firstRTIndex, const BlendState* states, const std::uint8_t stateCount);
			virtual void ApplyDepthStencilState(const DepthStencilState& state);
			virtual void ApplyViewports(const RectFList& vp);
			virtual void ApplyScissorRects(const RectFList& scissorRects);
			virtual void ApplyRenderTargets(const SharedRenderTargetPtr* renderTargets, const std::uint8_t targetCount, const IDepthStencilBuffer* dsBuffer);
			using DefaultShaderMap = std::unordered_map<ShaderType, SharedShaderPtr>;
			std::unique_ptr<IShaderManager> m_shaderMgr;
			PipelineState m_devicePipelineState;
			DefaultShaderMap m_defaultShaders;
		};
	}
}