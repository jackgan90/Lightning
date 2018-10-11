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
			void Draw(const RenderNode& item)override;
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply()override;
		protected:
			void CommitBuffers(const SharedGeometryPtr& geometry);
			void CommitPipelineStates(const RenderNode& item);
			void CommitShaderArguments(const RenderNode& item);
			void Draw(const SharedGeometryPtr& geometry);
			void GetInputLayouts(const SharedGeometryPtr& geometry, container::vector<VertexInputLayout>& layouts);
			using RenderNodeList = container::vector<RenderNode>;
			RenderNodeList mRenderNodes;
		};
	}
}
