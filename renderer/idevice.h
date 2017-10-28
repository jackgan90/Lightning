#pragma once
#include <memory>
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
		class LIGHTNINGGE_RENDERER_API IDevice
		{
		public:
			virtual ~IDevice() = default;
			//clear a specified render target,possibly parts of it defined by an array of rects
			virtual void ClearRenderTarget(IRenderTarget* rt, const ColorF& color, const RectI* pRects = nullptr, const int rectCount=0) = 0;
			virtual SharedVertexBufferPtr CreateVertexBuffer() = 0;
			virtual SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap) = 0;
			virtual void ApplyRasterizerState(const RasterizerState& state) = 0;
			virtual void ApplyBlendState(const BlendState& state) = 0;
			virtual void ApplyDepthStencilState(const DepthStencilState& state) = 0;
			virtual void ApplyPipelineState(const PipelineState& state) = 0;
		};
		typedef std::shared_ptr<IDevice> SharedDevicePtr;
	}
}
