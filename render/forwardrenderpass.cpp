#include <unordered_map>
#include "forwardrenderpass.h"
#include "renderer.h"

namespace Lightning
{
	namespace Render
	{
		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void ForwardRenderPass::Apply()
		{
			const auto& renderQueue = Renderer::Instance()->GetRenderQueue();
			for (const auto& node : renderQueue)
			{
				CommitShaderArguments(node);
				CommitPipelineStates(node);
				CommitBuffers(node.geometry);
				Draw(node.geometry);
			}
		}

		void ForwardRenderPass::CommitPipelineStates(const RenderNode& node)
		{
			static container::vector<VertexInputLayout> layouts;
			PipelineState state{};
			//TODO : set render target count based on model setting
			state.renderTargetCount = node.renderTargets.size();
			auto pSwapChain = Renderer::Instance()->GetSwapChain();
			state.renderTargets[0] = pSwapChain->GetDefaultRenderTarget().get();
			if (node.material)
			{
				state.vs = node.material->GetShader(ShaderType::VERTEX);
				state.fs = node.material->GetShader(ShaderType::FRAGMENT);
				state.gs = node.material->GetShader(ShaderType::GEOMETRY);
				state.hs = node.material->GetShader(ShaderType::TESSELATION_CONTROL);
				state.ds = node.material->GetShader(ShaderType::TESSELATION_EVALUATION);
				for (auto i = 0;i < state.renderTargetCount;++i)
				{
					state.blendStates[i] = node.material->GetBlendState();
					if (state.blendStates[i].enable)
					{
						state.depthStencilState.depthWriteEnable = false;
					}
				}
			}
			state.primType = node.geometry->primType;
			//TODO : Apply other pipeline states(blend state, rasterizer state etc)
			auto pDevice = Renderer::Instance()->GetDevice();
			layouts.clear();
			GetInputLayouts(node.geometry, layouts);
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
			if (node.depthStencilBuffer)
			{
				state.depthStencilState.bufferFormat = node.depthStencilBuffer->GetRenderFormat();
			}
			pDevice->ApplyRenderTargets(node.renderTargets, node.depthStencilBuffer);
			pDevice->ApplyPipelineState(state);
		}

		void ForwardRenderPass::CommitShaderArguments(const RenderNode& node)
		{
			if (!node.material)
				return;
			const auto& shaderMap = node.material->GetMaterialShaderMap();
			for (const auto& shaderAndArgs : shaderMap)
			{
				for (const auto& arg : shaderAndArgs.second.arguments)
				{
					shaderAndArgs.second.shader->SetArgument(arg);
				}
			}
			auto semantics = node.material->GetSemanticRequirements();
			for (auto semantic : semantics)
			{
				switch (semantic)
				{
				case RenderSemantics::WVP:
				{
					auto vs = node.material->GetShader(ShaderType::VERTEX);
					if (vs)
					{
						//We know that transform.ToMatrix4 may change it's internal matrix
						auto worldMatrix = const_cast<RenderNode&>(node).transform.LocalToGlobalMatrix4();
						auto wvp = node.projectionMatrix * node.viewMatrix * worldMatrix;
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
			for (std::uint8_t i = 0; i < MAX_GEOMETRY_BUFFER_COUNT; i++)
			{
				if (!geometry->vbs[i])
					continue;
				geometry->vbs[i]->Commit();
				pDevice->BindGPUBuffer(i, geometry->vbs[i]);
			}
			if (geometry->ib)
			{
				geometry->ib->Commit();
				pDevice->BindGPUBuffer(0, geometry->ib);
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