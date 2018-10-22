#include <unordered_map>
#include "forwardrenderpass.h"
#include "renderer.h"
#include "framememoryallocator.h"
#ifdef LIGHTNING_RENDER_MT
#include "tbb/flow_graph.h"
#include "tbb/parallel_for.h"
#endif


namespace Lightning
{
	namespace Render
	{
		extern Foundation::FrameMemoryAllocator g_RenderAllocator;
		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void ForwardRenderPass::Apply()
		{
			const auto& renderQueue = Renderer::Instance()->GetRenderQueue();
#ifdef LIGHTNING_RENDER_MT
			tbb::flow::graph g;
			tbb::flow::continue_node<tbb::flow::continue_msg> node(g, 
				[&renderQueue, this](const tbb::flow::continue_msg&) {
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
			});
			node.try_put(tbb::flow::continue_msg());
			g.wait_for_all();
#else
			for (const auto& node : renderQueue)
			{
				CommitShaderArguments(node);
				CommitPipelineStates(node);
				CommitBuffers(node.geometry);
				Draw(node.geometry);
			}
#endif
		}

		void ForwardRenderPass::CommitPipelineStates(const RenderNode& node)
		{
			PipelineState state{};
			//TODO : set render target count based on model setting
			state.renderTargetCount = static_cast<std::uint8_t>(node.renderTargets.size());
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
			
			GetInputLayouts(node.geometry, &state.inputLayouts, state.inputLayoutCount);

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
				layout.componentCount = geometry->vbs[i]->GetVertexComponentCount();
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