#pragma once
#include "RenderPass.h"

namespace Lightning
{
	namespace Render
	{
		class DeferedRenderPass : public RenderPass
		{
		public:	
			DeferedRenderPass():RenderPass(RenderPassType::DEFERED){}
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply(DrawCallQueue&)override;
			void OnFrameEnd()override;
		};
	}
}
