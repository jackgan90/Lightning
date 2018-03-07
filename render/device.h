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
			void ApplyRasterizerState(const RasterizerState& state)override;
			void ApplyBlendState(const BlendState& state)override;
			void ApplyDepthStencilState(const DepthStencilState& state)override;
			void ApplyPipelineState(const PipelineState& state)override;
			SharedShaderPtr GetDefaultShader(ShaderType type)override;
		protected:
			using DefaultShaderMap = std::unordered_map<ShaderType, SharedShaderPtr>;
			std::unique_ptr<IShaderManager> m_shaderMgr;
			PipelineState m_devicePipelineState;
			DefaultShaderMap m_defaultShaders;
		};
	}
}