#pragma once
#include "container.h"
#include "renderpass.h"
#include "pipelinestate.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::container;

		class LIGHTNING_RENDER_API ForwardRenderPass : public RenderPass
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
			void GetInputLayouts(const SharedGeometryPtr& geometry, container::vector<VertexInputLayout>& layouts);
			using RenderItemList = container::vector<RenderItem>;
			RenderItemList mRenderItems;
		};
	}
}
