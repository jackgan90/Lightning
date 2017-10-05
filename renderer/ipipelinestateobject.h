#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "iblendstate.h"
#include "idepthstencilstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API IPipelineStateObject
		{
		public:
			virtual ~IPipelineStateObject(){}
			virtual bool ApplyBlendState(const BlendStatePtr& blendState) = 0;
			virtual bool ApplyDepthStencilState(const DepthStencilStatePtr& dsState) = 0;
			virtual BlendStatePtr GetBlendState()const = 0;
			virtual DepthStencilStatePtr GetDepthStencilState()const = 0;
		};
		using PipelineStateObjectPtr = std::shared_ptr<IPipelineStateObject>;
	}
}