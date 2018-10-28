#include "d3d12forwardrenderpass.h"
#include "renderer.h"
#include "d3d12constantbuffermanager.h"
#include "d3d12descriptorheapmanager.h"
#include "d3d12shader.h"

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
			D3D12DescriptorHeapManager::Instance()->ReserveFrameDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				true, mTotalConstantBuffers);
			ForwardRenderPass::Apply();
		}

		void D3D12ForwardRenderPass::OnAddRenderNode(const RenderNode& node)
		{
			container::vector<IShader*> shaders;
			node.material->GetShaders(shaders);
			std::for_each(shaders.begin(), shaders.end(), [this](IShader* shader) {
				auto d3d12Shader = static_cast<D3D12Shader*>(shader);
				mTotalConstantBufferSize += d3d12Shader->GetConstantBufferSize();
				mTotalConstantBuffers += d3d12Shader->GetConstantBufferCount();
			});
		}

		void D3D12ForwardRenderPass::OnBeginFrame()
		{
			mTotalConstantBufferSize = 0;
			mTotalConstantBuffers = 0;
		}
	}
}