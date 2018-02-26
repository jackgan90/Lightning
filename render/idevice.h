#pragma once
#include <memory>
#include <vector>
#include "Eigen/StdVector"
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "idepthstencilbuffer.h"
#include "renderstates.h"
#include "vertexbuffer.h"
#include "ishader.h"
#include "rect.h"
#include "color.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API IDevice
		{
		public:
			virtual ~IDevice() = default;
			//clear a specified render target,possibly parts of it defined by an array of rects
			virtual void ClearRenderTarget(IRenderTarget* rt, const ColorF& color, const RectIList* rects=nullptr) = 0;
			virtual SharedVertexBufferPtr CreateVertexBuffer() = 0;
			virtual SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap) = 0;
			virtual void ApplyRasterizerState(const RasterizerState& state) = 0;
			virtual void ApplyBlendState(const BlendState& state) = 0;
			virtual void ApplyDepthStencilState(const DepthStencilState& state) = 0;
			virtual void ApplyPipelineState(const PipelineState& state) = 0;
			virtual void ApplyViewports(const RectFList& vp) = 0;
			virtual void ApplyScissorRects(const RectFList& scissorRects) = 0;
			virtual void ApplyRenderTargets(const RenderTargetList& renderTargets, const IDepthStencilBuffer* dsBuffer) = 0;
		};
		using SharedDevicePtr = std::shared_ptr<IDevice>;
	}
}
