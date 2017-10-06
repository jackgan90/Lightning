#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "irenderresourcekeeper.h"
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
		class LIGHTNINGGE_RENDERER_API IDevice : public IRenderResourceKeeper
		{
		public:
			//clear a specified render target,possiblly parts of it defined by an array of rects
			virtual void ClearRenderTarget(const RenderTargetPtr& rt, const ColorF& color, const RectI* pRects = nullptr, const int rectCount=0) = 0;
			virtual BlendStatePtr CreateBlendState() = 0;
			virtual DepthStencilStatePtr CreateDepthStencilState() = 0;
			virtual PipelineStateObjectPtr CreatePipelineStateObject() = 0;
			virtual VertexBufferPtr CreateVertexBuffer() = 0;
			virtual ShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap) = 0;
			virtual RasterizerStatePtr CreateRasterizerState() = 0;
		};
		using DevicePtr = std::shared_ptr<IDevice>;
	}
}
