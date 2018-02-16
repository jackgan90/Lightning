#pragma once
#include "rendererexportdef.h"
#include "geometry.h"
#include "material.h"

namespace LightningGE
{
	namespace Render
	{
		enum RenderPassType
		{
			RENDERPASS_FORWARD,
			RENDERPASS_DEFERED
		};

		class LIGHTNINGGE_RENDER_API RenderPass
		{
		public:
			virtual ~RenderPass(){}
			virtual void Draw(const SharedGeometryPtr& geometry, const SharedMaterialPtr& material) = 0;
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			virtual void Apply() = 0;
			RenderPassType GetType()const { return m_type; }
		protected:
			RenderPass(RenderPassType type):m_type(type){}
			RenderPassType m_type;
		};
	}
}