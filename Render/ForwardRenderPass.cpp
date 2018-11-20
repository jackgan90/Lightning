#include <unordered_map>
#include "ForwardRenderPass.h"
#include "Renderer.h"
#include "FrameMemoryAllocator.h"
#include "Material.h"
#include "tbb/flow_graph.h"
#include "tbb/parallel_for.h"


namespace Lightning
{
	namespace Render
	{
		extern FrameMemoryAllocator g_RenderAllocator;
		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void ForwardRenderPass::Apply()
		{
			const auto& renderQueue = Renderer::Instance()->GetRenderQueue();
			tbb::parallel_for(tbb::blocked_range<std::size_t>(0, renderQueue.size()), 
				[&renderQueue, this](const tbb::blocked_range<std::size_t>& range) {
				for (std::size_t i = range.begin(); i != range.end();++i)
				{
					const auto& node = renderQueue[i];
					CommitShaderArguments(node);
					CommitPipelineStates(node);
					CommitBuffers(node.geometry);
					Draw(node.geometry);
				}
			});
		}

		void ForwardRenderPass::OnAddRenderNode(const RenderNode& node)
		{

		}

		void ForwardRenderPass::OnFrameBegin()
		{
		}

		void ForwardRenderPass::OnFrameUpdate()
		{
			Apply();
		}

		void ForwardRenderPass::CommitPipelineStates(const RenderNode& node)
		{
			PipelineState state{};
			//TODO : set render target count based on model setting
			state.renderTargetCount = static_cast<std::uint8_t>(node.renderTargets.size());
			auto renderer = Renderer::Instance();
			auto pSwapChain = renderer->GetSwapChain();
			state.renderTargets[0] = pSwapChain->GetDefaultRenderTarget().get();
			if (node.material)
			{
				auto material = static_cast<Material*>(node.material);
				state.vs = material->GetShader(ShaderType::VERTEX);
				state.fs = material->GetShader(ShaderType::FRAGMENT);
				state.gs = material->GetShader(ShaderType::GEOMETRY);
				state.hs = material->GetShader(ShaderType::TESSELATION_CONTROL);
				state.ds = material->GetShader(ShaderType::TESSELATION_EVALUATION);
				for (auto i = 0;i < state.renderTargetCount;++i)
				{
					state.blendStates[i] = material->GetBlendState();
					if (state.blendStates[i].enable)
					{
						state.depthStencilState.depthWriteEnable = false;
					}
				}
			}
			state.primType = node.geometry->primType;
			//TODO : Apply other pipeline states(blend state, rasterizer state etc)
			
			GetInputLayouts(node.geometry, &state.inputLayouts, state.inputLayoutCount);

			if (node.depthStencilBuffer)
			{
				state.depthStencilState.bufferFormat = node.depthStencilBuffer->GetRenderFormat();
			}
			renderer->ApplyRenderTargets(node.renderTargets, node.depthStencilBuffer);
			renderer->ApplyPipelineState(state);
		}

		void ForwardRenderPass::CommitShaderArguments(const RenderNode& node)
		{
			if (!node.material)
				return;
			auto material = static_cast<Material*>(node.material);
			const auto& shaderMap = material->GetMaterialShaderMap();
			for (const auto& shaderAndArgs : shaderMap)
			{
				for (const auto& arg : shaderAndArgs.second.arguments)
				{
					shaderAndArgs.second.shader->SetArgument(arg);
				}
			}
			auto semantics = material->GetSemanticRequirements();
			for (auto semantic : semantics)
			{
				switch (semantic)
				{
				case RenderSemantics::WVP:
				{
					auto vs = material->GetShader(ShaderType::VERTEX);
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

		void ForwardRenderPass::GetInputLayouts(const SharedGeometryPtr& geometry, VertexInputLayout** layouts, std::uint8_t& layoutCount)
		{
			*layouts = nullptr;
			layoutCount = 0;
			for (std::size_t i = 0;i < MAX_GEOMETRY_BUFFER_COUNT;i++)
			{
				if (!geometry->vbs[i])
					continue;
				if (*layouts == nullptr)
					*layouts = g_RenderAllocator.Allocate<VertexInputLayout>(MAX_GEOMETRY_BUFFER_COUNT);
				auto& layout = (*layouts)[layoutCount];
				layout.slot = static_cast<std::uint8_t>(i);
				layout.componentCount = static_cast<std::uint8_t>(geometry->vbs[i]->GetVertexComponentCount());
				layout.components = nullptr;
				if (layout.componentCount > 0)
				{
					layout.components = g_RenderAllocator.Allocate<VertexComponent>(layout.componentCount);
					for (std::size_t j = 0;j < layout.componentCount;++j)
					{
						layout.components[j] = geometry->vbs[i]->GetVertexComponent(j);
					}
				}
				++layoutCount;
			}
		}

		void ForwardRenderPass::CommitBuffers(const SharedGeometryPtr& geometry)
		{
			auto renderer = Renderer::Instance();
			for (std::uint8_t i = 0; i < MAX_GEOMETRY_BUFFER_COUNT; i++)
			{
				if (!geometry->vbs[i])
					continue;
				geometry->vbs[i]->Commit();
				renderer->BindGPUBuffer(i, geometry->vbs[i]);
			}
			if (geometry->ib)
			{
				geometry->ib->Commit();
				renderer->BindGPUBuffer(0, geometry->ib);
			}
		}

		void ForwardRenderPass::Draw(const SharedGeometryPtr& geometry)
		{
			auto renderer = Renderer::Instance();
			if (geometry->ib)
			{
				DrawParam param{};
				param.drawType = DrawType::Index;
				param.indexCount = geometry->ib->GetIndexCount();
				param.instanceCount = 1;
				renderer->Draw(param);
			}
			else
			{
				//TODO : should implement
			}
		}

	}
}