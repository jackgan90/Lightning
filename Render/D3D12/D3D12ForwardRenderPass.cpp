#include "D3D12ForwardRenderPass.h"
#include "Renderer.h"
#include "D3D12ConstantBufferManager.h"
#include "D3D12DescriptorHeapManager.h"
#include "ThreadLocalObject.h"
#include "D3D12Shader.h"

namespace Lightning
{
	namespace Render
	{
		D3D12ForwardRenderPass::D3D12ForwardRenderPass()
			: mTotalConstantBufferSize(0), mTotalConstantBuffers(0)
			, mTotalSamplerStates(0), mTotalTextures(0)
		{
		}

		void D3D12ForwardRenderPass::Apply(RenderQueue& renderQueue)
		{
			if (mTotalConstantBuffers + mTotalTextures > 0)
			{
				D3D12DescriptorHeapManager::Instance()->ReserveFrameDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
					true, mTotalConstantBuffers + mTotalTextures);
			}
			if (mTotalSamplerStates)
			{
				D3D12DescriptorHeapManager::Instance()->ReserveFrameDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
					true, mTotalSamplerStates);
			}
			ForwardRenderPass::Apply(renderQueue);
		}

		void D3D12ForwardRenderPass::OnAddRenderUnit(const IImmutableRenderUnit* unit)
		{
			using ShaderVec = std::vector<std::shared_ptr<IShader>>;
			static Foundation::ThreadLocalObject<ShaderVec> tloShaders;
			auto& shaders = *tloShaders;
			shaders.clear();
			GetMaterialShaders(unit->GetMaterial().get(), shaders);
			std::for_each(shaders.begin(), shaders.end(), [this](const std::shared_ptr<IShader>& shader) {
				auto d3d12Shader = std::static_pointer_cast<D3D12Shader>(shader);
				mTotalConstantBufferSize += d3d12Shader->GetConstantBufferSize();
				mTotalConstantBuffers += d3d12Shader->GetConstantBufferCount();
				mTotalSamplerStates += d3d12Shader->GetSamplerStateCount();
				mTotalTextures += d3d12Shader->GetTextureCount();
			});
		}
		
		void D3D12ForwardRenderPass::GetMaterialShaders(IMaterial* material, std::vector<std::shared_ptr<IShader>>& shaders)
		{
			auto vs = material->GetShader(ShaderType::VERTEX);
			auto fs = material->GetShader(ShaderType::FRAGMENT);
			auto gs = material->GetShader(ShaderType::GEOMETRY);
			auto hs = material->GetShader(ShaderType::HULL);
			auto ds = material->GetShader(ShaderType::DOMAIN);
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
			mTotalSamplerStates = 0;
			mTotalTextures = 0;
		}
	}
}