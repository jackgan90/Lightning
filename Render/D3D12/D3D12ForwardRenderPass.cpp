#include "D3D12ForwardRenderPass.h"
#include "Renderer.h"
#include "D3D12ConstantBufferManager.h"
#include "D3D12DescriptorHeapManager.h"
#include "ThreadLocalSingleton.h"
#include "D3D12Shader.h"

namespace Lightning
{
	namespace Render
	{
		D3D12ForwardRenderPass::D3D12ForwardRenderPass():
			mTotalConstantBufferSize(0), mTotalConstantBuffers(0)
		{
		}

		void D3D12ForwardRenderPass::Apply(RenderQueue& renderQueue)
		{
			auto frameResourceIndex = Renderer::Instance()->GetFrameResourceIndex();
			if(mTotalConstantBufferSize > 0)
				D3D12ConstantBufferManager::Instance()->Reserve(mTotalConstantBufferSize);
			if(mTotalConstantBuffers)
				D3D12DescriptorHeapManager::Instance()->ReserveFrameDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
					true, mTotalConstantBuffers);
			ForwardRenderPass::Apply(renderQueue);
		}

		void D3D12ForwardRenderPass::OnAddRenderUnit(const IRenderUnit* unit)
		{
			using ShaderContainer = Container::Vector<IShader*>;
			static Foundation::ThreadLocalSingleton<ShaderContainer> shadersContainer;
			auto& shaders = *shadersContainer;
			shaders.clear();
			GetMaterialShaders(unit->GetMaterial(), shaders);
			std::for_each(shaders.begin(), shaders.end(), [this](IShader* shader) {
				auto d3d12Shader = static_cast<D3D12Shader*>(shader);
				mTotalConstantBufferSize += d3d12Shader->GetConstantBufferSize();
				mTotalConstantBuffers += d3d12Shader->GetConstantBufferCount();
			});
		}
		
		void D3D12ForwardRenderPass::GetMaterialShaders(IMaterial* material, Container::Vector<IShader*>& shaders)
		{
			auto vs = material->GetShader(ShaderType::VERTEX);
			auto fs = material->GetShader(ShaderType::FRAGMENT);
			auto gs = material->GetShader(ShaderType::GEOMETRY);
			auto hs = material->GetShader(ShaderType::TESSELATION_CONTROL);
			auto ds = material->GetShader(ShaderType::TESSELATION_EVALUATION);
			if (vs)
				shaders.push_back(vs);
			if (fs)
				shaders.push_back(fs);
			if (gs)
				shaders.push_back(gs);
			if (hs)
				shaders.push_back(hs);
			if (ds)
				shaders.push_back(ds);
		}

		void D3D12ForwardRenderPass::OnFrameEnd()
		{
			mTotalConstantBufferSize = 0;
			mTotalConstantBuffers = 0;
		}
	}
}