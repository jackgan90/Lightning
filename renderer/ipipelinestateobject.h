#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "irenderresourcekeeper.h"
#include "iblendstate.h"
#include "idepthstencilstate.h"
#include "irasterizerstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IPipelineStateObject : public IRenderResourceKeeper
		{
		public:
			virtual bool ApplyBlendState(const BlendStatePtr& blendState) = 0;
			virtual bool ApplyDepthStencilState(const DepthStencilStatePtr& dsState) = 0;
			virtual bool ApplyRasterizerState(const RasterizerStatePtr& rasterizerState) = 0;
			virtual BlendStatePtr GetBlendState()const = 0;
			virtual DepthStencilStatePtr GetDepthStencilState()const = 0;
			virtual RasterizerStatePtr GetRasterizerState()const = 0;
		};
		using PipelineStateObjectPtr = std::shared_ptr<IPipelineStateObject>;
	}
}