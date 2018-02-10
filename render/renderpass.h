#pragma once
#include "rendererexportdef.h"
#include "geometry.h"
#include "material.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API RenderPass
		{
		public:
			virtual ~RenderPass(){}
			virtual void Draw(const SharedGeometryPtr& geometry, const SharedMaterialPtr& material) = 0;
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			virtual void Apply() = 0;
		};
	}
}