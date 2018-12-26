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
			void OnAddRenderUnit(const IRenderUnit* unit)override;
			void OnFrameEnd()override;
		private:
			void GetMaterialShaders(IMaterial* material, Container::Vector<IShader*>& shaders);
			std::size_t mTotalConstantBufferSize;
			std::uint32_t mTotalConstantBuffers;
		};
	}
}