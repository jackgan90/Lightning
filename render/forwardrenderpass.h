#pragma once
#include "renderpass.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API ForwardRenderPass : public RenderPass
		{
		public:	
			void Draw(const SharedGeometryPtr& geometry, const SharedMaterialPtr& material)override;
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply()override;
		};
	}
}
