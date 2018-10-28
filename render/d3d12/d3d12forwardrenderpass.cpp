#include "d3d12forwardrenderpass.h"
#include "renderer.h"
#include "d3d12constantbuffermanager.h"

namespace Lightning
{
	namespace Render
	{
		D3D12ForwardRenderPass::D3D12ForwardRenderPass():
			mTotalConstantBufferSize(0)
		{
		}

		void D3D12ForwardRenderPass::Apply()
		{
			auto frameResourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			D3D12ConstantBufferManager::Instance()->Reserve(mTotalConstantBufferSize);
			ForwardRenderPass::Apply();
		}

		void D3D12ForwardRenderPass::OnAddRenderNode(const RenderNode& node)
		{
			container::vector<IShader*> shaders;
			node.material->GetShaders(shaders);
			std::for_each(shaders.begin(), shaders.end(), [this](IShader* shader) {
				mTotalConstantBufferSize += shader->GetConstantBufferSize();
			});
		}

		void D3D12ForwardRenderPass::OnBeginFrame()
		{
			mTotalConstantBufferSize = 0;
		}
	}
}