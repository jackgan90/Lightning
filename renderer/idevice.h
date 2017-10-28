#pragma once
#include <memory>
#include <vector>
#include "Eigen/StdVector"
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "renderstates.h"
#include "ivertexbuffer.h"
#include "ishader.h"
#include "rect.h"
#include "color.h"

namespace LightningGE
{
	namespace Renderer
	{
		template<typename T>
		using EigenVector = std::vector<T, Eigen::aligned_allocator<T>>;
		using RenderIRects = EigenVector<RectI>;
		using RenderFRects = EigenVector<RectF>;
		using RenderViewports = EigenVector<Viewport>;
		using RenderScissorRects = EigenVector<ScissorRect>;
		class LIGHTNINGGE_RENDERER_API IDevice
		{
		public:
			virtual ~IDevice() = default;
			//clear a specified render target,possibly parts of it defined by an array of rects
			virtual void ClearRenderTarget(IRenderTarget* rt, const ColorF& color, const RenderIRects* rects=nullptr) = 0;
			virtual SharedVertexBufferPtr CreateVertexBuffer() = 0;
			virtual SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderFileName, const ShaderDefine& defineMap) = 0;
			virtual void ApplyRasterizerState(const RasterizerState& state) = 0;
			virtual void ApplyBlendState(const BlendState& state) = 0;
			virtual void ApplyDepthStencilState(const DepthStencilState& state) = 0;
			virtual void ApplyPipelineState(const PipelineState& state) = 0;
			virtual void ApplyViewports(const RenderViewports& vp) = 0;
			virtual void ApplyScissorRects(const RenderScissorRects& scissorRects) = 0;
		};
		typedef std::shared_ptr<IDevice> SharedDevicePtr;
	}
}
