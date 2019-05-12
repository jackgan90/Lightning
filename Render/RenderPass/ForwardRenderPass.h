#pragma once
#include "RenderPass.h"
#include "PipelineState.h"

namespace Lightning
{
	namespace Render
	{
		class ForwardRenderPass : public RenderPass
		{
		public:	
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply(DrawCommandQueue& drawCommands)override;
		};
	}
}
