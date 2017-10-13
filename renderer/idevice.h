#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "iblendstate.h"
#include "idepthstencilstate.h"
#include "irasterizerstate.h"
#include "ipipelinestateobject.h"
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
			virtual void ClearRenderTarget(const SharedRenderTargetPtr& rt, const ColorF& color, const RectI* pRects = nullptr, const int rectCount=0) = 0;
			virtual SharedBlendStatePtr CreateBlendState() = 0;
			virtual SharedDepthStencilStatePtr CreateDepthStencilState() = 0;
			virtual SharedPipelineStateObjectPtr CreatePipelineStateObject() = 0;
			virtual SharedVertexBufferPtr CreateVertexBuffer() = 0;
			virtual SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap) = 0;
			virtual SharedRasterizerStatePtr CreateRasterizerState() = 0;
		};
		typedef std::shared_ptr<IDevice> SharedDevicePtr;
	}
}
