#pragma once
#include "renderpass.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API DeferedRenderPass : public RenderPass
		{
		public:	
			DeferedRenderPass():RenderPass(RenderPassType::DEFERED){}
			void Draw(const RenderItem& item)override;
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply()override;
		};
	}
}
