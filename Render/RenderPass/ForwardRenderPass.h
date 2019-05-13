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
			ForwardRenderPass();
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			void Render(IRenderer& renderer)override;
			std::size_t GetRenderTargetCount()const override;
			std::shared_ptr<IRenderTarget> GetRenderTarget(std::size_t index)const override;
			std::shared_ptr<IDepthStencilBuffer> GetDepthStencilBuffer()const override;
		protected:
			bool AcceptDrawable(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera)override;
			ColorF mClearColor;
		};
	}
}
