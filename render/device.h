#pragma once
#include <memory>
#include "idevice.h"
#include "ishadermanager.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API Device : public IDevice
		{
		public:
			friend class Renderer;
			Device();
			void ApplyPipelineState(const PipelineState& state)override;
			SharedShaderPtr GetDefaultShader(ShaderType type)override;
			void BeginFrame(const std::size_t frameResourceIndex)override;
		protected:
			virtual void ApplyRasterizerState(const RasterizerState& state);
			virtual void ApplyBlendStates(const std::uint8_t firstRTIndex, const BlendState* states, const std::uint8_t stateCount);
			virtual void ApplyDepthStencilState(const DepthStencilState& state);
			virtual void ApplyViewports(const RectFList& vp);
			virtual void ApplyScissorRects(const RectFList& scissorRects);
			using DefaultShaderMap = container::unordered_map<ShaderType, SharedShaderPtr>;
			std::unique_ptr<IShaderManager> mShaderMgr;
			PipelineState mDevicePipelineState;
			DefaultShaderMap mDefaultShaders;
			std::size_t mFrameResourceIndex;
		};
	}
}
