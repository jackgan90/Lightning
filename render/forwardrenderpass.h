#pragma once
#include <vector>
#include "renderpass.h"
#include "renderstates.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API ForwardRenderPass : public RenderPass
		{
		public:	
			ForwardRenderPass():RenderPass(RenderPassType::FORWARD){}
			void Draw(const RenderItem& item)override;
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply()override;
		protected:
			void CommitBuffers(const SharedGeometryPtr& geometry);
			void CommitPipelineStates(const RenderItem& item);
			void CommitShaderArguments(const RenderItem& item);
			void Draw(const SharedGeometryPtr& geometry);
			std::vector<VertexInputLayout> GetInputLayouts(const SharedGeometryPtr& geometry);
			using RenderItemList = std::vector<RenderItem>;
			RenderItemList m_renderItems;
		};
	}
}
