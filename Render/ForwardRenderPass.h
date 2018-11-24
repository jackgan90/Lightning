#pragma once
#include "Container.h"
#include "RenderPass.h"
#include "PipelineState.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;

		class ForwardRenderPass : public RenderPass
		{
		public:	
			ForwardRenderPass():RenderPass(RenderPassType::FORWARD){}
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply(RenderQueue& renderQueue)override;
			void OnAddRenderNode(const RenderNode& node)override;
			void OnFrameBegin()override;
		protected:
			void CommitBuffers(const Geometry& geometry);
			void CommitPipelineStates(const RenderNode& item);
			void CommitShaderArguments(const RenderNode& item);
			void Draw(const Geometry& geometry);
			void GetInputLayouts(const Geometry& geometry, VertexInputLayout** layouts, std::uint8_t& layoutCount);
		};
	}
}
