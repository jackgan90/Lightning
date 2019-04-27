#pragma once
#include "RenderPass.h"
#include "PipelineState.h"

namespace Lightning
{
	namespace Render
	{
		class ForwardRenderPass : public RenderPass
		{
		public:	
			ForwardRenderPass():RenderPass(RenderPassType::FORWARD){}
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Apply(RenderQueue& renderQueue)override;
			void OnAddRenderUnit(const IImmutableRenderUnit* unit)override;
			void OnFrameEnd()override;
		protected:
			void CommitBuffers(const IImmutableRenderUnit* unit);
			void CommitPipelineStates(const IImmutableRenderUnit* unit);
			void CommitShaderParameters(const IImmutableRenderUnit* unit);
			void CommitSemanticUniforms(IShader* shader, const IImmutableRenderUnit* unit);
			void Draw(const IImmutableRenderUnit* unit);
			void GetInputLayouts(const IImmutableRenderUnit* unit, VertexInputLayout*& layouts, std::size_t& layoutCount);
		};
	}
}
