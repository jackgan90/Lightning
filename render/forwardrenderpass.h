#pragma once
#include <vector>
#include "renderpass.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API ForwardRenderPass : public RenderPass
		{
		public:	
			ForwardRenderPass():RenderPass(RenderPassType::FORWARD){}
			void Draw(const SharedGeometryPtr& geometry, const SharedMaterialPtr& material, const Transform& transform)override;
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply()override;
		protected:
			void CommitBuffers(const SharedGeometryPtr& geometry);
			struct RenderItem
			{
				RenderItem(const SharedGeometryPtr& geo, const SharedMaterialPtr& mtl):geometry(geo), material(mtl){}
				SharedGeometryPtr geometry;
				SharedMaterialPtr material;
			};
			using RenderItemList = std::vector<RenderItem>;
			RenderItemList m_renderItems;
		};
	}
}
