#pragma once
#include "ForwardRenderPass.h"

namespace Lightning
{
	namespace Render
	{
		class D3D12ForwardRenderPass : public ForwardRenderPass
		{
		public:
			D3D12ForwardRenderPass();
			void Apply(RenderQueue&)override;
			void OnAddRenderNode(const RenderNode& node)override;
			void OnFrameEnd()override;
		private:
			std::size_t mTotalConstantBufferSize;
			std::uint32_t mTotalConstantBuffers;
		};
	}
}