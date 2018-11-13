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
			void Apply()override;
			void OnAddRenderNode(const RenderNode& node)override;
			void OnFrameBegin()override;
		private:
			std::size_t mTotalConstantBufferSize;
			std::uint32_t mTotalConstantBuffers;
		};
	}
}