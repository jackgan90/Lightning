#include <unordered_map>
#include "forwardrenderpass.h"
#include "renderer.h"

namespace Lightning
{
	namespace Render
	{
		void ForwardRenderPass::Draw(const RenderItem& item)
		{
			mRenderItems.push_back(item);
		}

		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void ForwardRenderPass::Apply()
		{
			for (auto& renderItem : mRenderItems)
			{
				CommitShaderArguments(renderItem);
				CommitPipelineStates(renderItem);
				CommitBuffers(renderItem.geometry);
				Draw(renderItem.geometry);
			}
			mRenderItems.clear();
		}

		void ForwardRenderPass::CommitPipelineStates(const RenderItem& item)
		{
			static container::vector<VertexInputLayout> layouts;
			PipelineState state{};
			//TODO : set render target count based on model setting
			state.outputRenderTargetCount = item.renderTargets;
			auto pSwapChain = Renderer::Instance()->GetSwapChain();
			state.renderTargets[0] = pSwapChain->GetPrimaryRenderTarget().get();
			if (item.material)
			{
				state.vs = item.material->GetShader(ShaderType::VERTEX);
				state.fs = item.material->GetShader(ShaderType::FRAGMENT);
				state.gs = item.material->GetShader(ShaderType::GEOMETRY);
				state.hs = item.material->GetShader(ShaderType::TESSELATION_CONTROL);
				state.ds = item.material->GetShader(ShaderType::TESSELATION_EVALUATION);
				for (auto i = 0;i < item.renderTargets;++i)
				{
					state.blendStates[i] = item.material->GetBlendState();
					if (state.blendStates[i].enable)
					{
						state.depthStencilState.depthWriteEnable = false;
						state.depthStencilState.depthTestEnable = false;
					}
				}
			}
			state.primType = item.geometry->primType;
			//TODO : Apply other pipeline states(blend state, rasterizer state etc)
			auto pDevice = Renderer::Instance()->GetDevice();
			layouts.clear();
			GetInputLayouts(item.geometry, layouts);
			if (!layouts.empty())
			{
				state.inputLayouts = &layouts[0];
				state.inputLayoutCount = static_cast<std::uint8_t>(layouts.size());
			}
			else
			{
				state.inputLayouts = nullptr;
				state.inputLayoutCount = 0;
			}
			pDevice->ApplyPipelineState(state);
		}

		void ForwardRenderPass::CommitShaderArguments(const RenderItem& item)
		{
			if (!item.material)
				return;
			const auto& shaderMap = item.material->GetMaterialShaderMap();
			for (const auto& shaderAndArgs : shaderMap)
			{
				for (const auto& arg : shaderAndArgs.second.arguments)
				{
					shaderAndArgs.second.shader->SetArgument(arg);
				}
			}
			auto semantics = item.material->GetSemanticRequirements();
			for (auto semantic : semantics)
			{
				switch (semantic)
				{
				case RenderSemantics::WVP:
				{
					auto vs = item.material->GetShader(ShaderType::VERTEX);
					if (vs)
					{
						auto worldMatrix = item.transform.GetTransformMatrix();
						auto wvp = item.projectionMatrix * item.viewMatrix * worldMatrix;
						vs->SetArgument(ShaderArgument("wvp", wvp));
					}
					break;
				}
				default:
					break;
				}
			}
		}

		void ForwardRenderPass::GetInputLayouts(const SharedGeometryPtr& geometry, container::vector<VertexInputLayout>& layouts)
		{
			static container::array<container::vector<VertexComponent>, MAX_GEOMETRY_BUFFER_COUNT> components;
			for (std::size_t i = 0;i < MAX_GEOMETRY_BUFFER_COUNT;i++)
			{
				if (!geometry->vbs[i])
					continue;
				components[i].clear();
				VertexInputLayout layout;
				layout.slot = static_cast<std::uint8_t>(i);
				for (std::size_t j = 0;j < geometry->vbs[i]->GetVertexComponentCount();++j)
				{
					components[i].push_back(geometry->vbs[i]->GetVertexComponent(j));
				}
				if (!components[i].empty())
				{
					layout.components = &components[i][0];
					layout.componentCount = static_cast<std::uint8_t>(components[i].size());
				}
				else
				{
					layout.components = nullptr;
					layout.componentCount = 0;
				}
				layouts.push_back(layout);
			}
		}

		void ForwardRenderPass::CommitBuffers(const SharedGeometryPtr& geometry)
		{
			auto pDevice = Renderer::Instance()->GetDevice();
			container::unordered_map<std::uint8_t, container::vector<SharedGPUBufferPtr>> bufferBinding;
			for (std::uint8_t i = 0; i < MAX_GEOMETRY_BUFFER_COUNT; i++)
			{
				if (!geometry->vbs[i])
					continue;
				if (geometry->vbs[i]->IsDirty())
				{
					pDevice->CommitGPUBuffer(geometry->vbs[i]);
					geometry->vbs[i]->ResetDirty();
				}
				if (bufferBinding.empty())
				{
					bufferBinding.insert(std::make_pair(i, container::vector<SharedGPUBufferPtr>()));
					bufferBinding[i].push_back(geometry->vbs[i]);
				}
				else
				{
					if (bufferBinding.find(i - 1) != bufferBinding.end())
						bufferBinding[i - 1].push_back(geometry->vbs[i]);
					else
					{
						bufferBinding.insert(std::make_pair(i, container::vector<SharedGPUBufferPtr>()));
						bufferBinding[i].push_back(geometry->vbs[i]);
					}
				}
			}
			if (geometry->ib)
			{
				if (geometry->ib->IsDirty())
				{
					pDevice->CommitGPUBuffer(geometry->ib);
					geometry->ib->ResetDirty();
				}
				bufferBinding.insert(std::make_pair(MAX_GEOMETRY_BUFFER_COUNT, container::vector<SharedGPUBufferPtr>()));
				bufferBinding[MAX_GEOMETRY_BUFFER_COUNT].push_back(geometry->ib);
			}
			for (auto it = bufferBinding.cbegin();it != bufferBinding.cend();++it)
			{
				pDevice->BindGPUBuffers(it->first, it->second);
			}
		}

		void ForwardRenderPass::Draw(const SharedGeometryPtr& geometry)
		{
			auto pDevice = Renderer::Instance()->GetDevice();
			if (geometry->ib)
			{
				pDevice->DrawIndexed(geometry->ib->GetIndexCount(), 1, 0, 0, 0);
			}
			else
			{
				//TODO : should implement
			}
		}

	}
}