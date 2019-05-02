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
			ForwardRenderPass():RenderPass(RenderPassType::FORWARD){}
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply(RenderQueue& renderQueue)override;
			void OnAddRenderUnit(const IImmutableRenderUnit* unit)override;
			void OnFrameEnd()override;
		};
	}
}
