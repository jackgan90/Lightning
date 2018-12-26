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
			void OnAddRenderUnit(const IRenderUnit* unit)override;
			void OnFrameEnd()override;
		protected:
			void CommitBuffers(const IRenderUnit* unit);
			void CommitPipelineStates(const IRenderUnit* unit);
			void CommitShaderParameters(const IRenderUnit* unit);
			void CommitSemanticUniforms(IShader* shader, const IRenderUnit* unit);
			void Draw(const IRenderUnit* unit);
			void GetInputLayouts(const IRenderUnit* unit, VertexInputLayout* layouts, std::size_t& layoutCount);
		};
	}
}
