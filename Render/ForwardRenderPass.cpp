#include "ForwardRenderPass.h"
#include "Renderer.h"
#include "FrameMemoryAllocator.h"
#include "tbb/flow_graph.h"
#include "tbb/parallel_for.h"


namespace Lightning
{
	namespace Render
	{
		extern FrameMemoryAllocator g_RenderAllocator;
		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void ForwardRenderPass::Apply(RenderQueue& renderQueue)
		{
			tbb::parallel_for(tbb::blocked_range<std::size_t>(0, renderQueue.size()), 
				[&renderQueue, this](const tbb::blocked_range<std::size_t>& range) {
				for (std::size_t i = range.begin(); i != range.end();++i)
				{
					const auto& node = renderQueue[i];
					CommitShaderParameters(node);
					CommitPipelineStates(node);
					CommitBuffers(node.geometry);
					Draw(node.geometry);
				}
			});
		}

		void ForwardRenderPass::OnAddRenderNode(const RenderNode& node)
		{

		}

		void ForwardRenderPass::OnFrameEnd()
		{
		}

		void ForwardRenderPass::CommitPipelineStates(const RenderNode& node)
		{
			PipelineState state;
			state.Reset();
			state.renderTargetCount = node.renderTargetCount;
			auto renderer = Renderer::Instance();
			auto pSwapChain = renderer->GetSwapChain();
			for (auto i = 0;i < node.renderTargetCount;++i)
			{
				state.renderTargets[i] = node.renderTargets[i];
			}
			if (node.material)
			{
				state.vs = node.material->GetShader(ShaderType::VERTEX);
				state.fs = node.material->GetShader(ShaderType::FRAGMENT);
				state.gs = node.material->GetShader(ShaderType::GEOMETRY);
				state.hs = node.material->GetShader(ShaderType::TESSELATION_CONTROL);
				state.ds = node.material->GetShader(ShaderType::TESSELATION_EVALUATION);
				for (auto i = 0;i < state.renderTargetCount;++i)
				{
					node.material->GetBlendState(state.blendStates[i]);
					if (state.blendStates[i].enable)
					{
						state.depthStencilState.depthWriteEnable = false;
					}
				}
			}
			state.primType = node.geometry.primType;
			//TODO : Apply other pipeline states(blend state, rasterizer state etc)
			
			GetInputLayouts(node.geometry, state.inputLayouts, state.inputLayoutCount);

			if (node.depthStencilBuffer)
			{
				state.depthStencilState.bufferFormat = node.depthStencilBuffer->GetRenderFormat();
			}
			renderer->ApplyRenderTargets(node.renderTargets, node.renderTargetCount, node.depthStencilBuffer);
			renderer->ApplyPipelineState(state);
		}

		void ForwardRenderPass::CommitShaderParameters(const RenderNode& node)
		{
			if (!node.material)
				return;
			static const ShaderType shaderTypes[] = { ShaderType::VERTEX, ShaderType::FRAGMENT, ShaderType::GEOMETRY,
			ShaderType::TESSELATION_CONTROL, ShaderType::TESSELATION_EVALUATION };
			auto renderer = Renderer::Instance();
			for (auto shaderType : shaderTypes)
			{
				IShader* shader = node.material->GetShader(shaderType);
				if (shader)
				{
					auto parameterCount = node.material->GetParameterCount(shaderType);
					for (auto i = 0;i < parameterCount;++i)
					{
						auto parameter = node.material->GetParameter(shaderType, i);
						shader->SetParameter(parameter);
					}
					CommitShaderUniforms(shader, node);
				}
			}
		}

		void ForwardRenderPass::CommitShaderUniforms(IShader* shader, const RenderNode& node)
		{
			auto renderer = Renderer::Instance();
			RenderSemantics* semantics{ nullptr };
			std::uint16_t semanticCount{ 0 };
			shader->GetUniformSemantics(&semantics, semanticCount);
			if (semanticCount == 0)
				return;
			for (auto i = 0;i < semanticCount;++i)
			{
				auto semantic = semantics[i];
				auto uniformName = renderer->GetUniformName(semantic);
				switch (semantic)
				{
				case RenderSemantics::WVP:
				{
					//We know that transform.ToMatrix4 may change it's internal matrix
					auto wvp = node.projectionMatrix * node.viewMatrix * node.transform.matrix;
					shader->SetParameter(&ShaderParameter(uniformName, wvp));
					break;
				}
				default:
					LOG_WARNING("Unsupported semantics : {0}", semantic);
					break;
				}
			}
		}

		void ForwardRenderPass::GetInputLayouts(const Geometry& geometry, VertexInputLayout* layouts, std::uint8_t& layoutCount)
		{
			layoutCount = 0;
			for (std::size_t i = 0;i < Foundation::ArraySize(geometry.vbs);i++)
			{
				if (!geometry.vbs[i])
					continue;
				auto& layout = layouts[layoutCount];
				layout.slot = static_cast<std::uint8_t>(i);
				layout.componentCount = static_cast<std::uint8_t>(geometry.vbs[i]->GetVertexComponentCount());
				assert(layout.componentCount <= MAX_INPUT_LAYOUT_COMPONENT_COUNT && "Input layout component count too large!");
				if (layout.componentCount > 0)
				{
					for (std::size_t j = 0;j < layout.componentCount;++j)
					{
						layout.components[j] = geometry.vbs[i]->GetVertexComponent(j);
					}
				}
				++layoutCount;
			}
		}

		void ForwardRenderPass::CommitBuffers(const Geometry& geometry)
		{
			auto renderer = Renderer::Instance();
			for (std::uint8_t i = 0; i < Foundation::ArraySize(geometry.vbs); i++)
			{
				if (!geometry.vbs[i])
					continue;
				geometry.vbs[i]->Commit();
				renderer->BindGPUBuffer(i, geometry.vbs[i]);
			}
			if (geometry.ib)
			{
				geometry.ib->Commit();
				renderer->BindGPUBuffer(0, geometry.ib);
			}
		}

		void ForwardRenderPass::Draw(const Geometry& geometry)
		{
			auto renderer = Renderer::Instance();
			if (geometry.ib)
			{
				DrawParam param{};
				param.drawType = DrawType::Index;
				param.indexCount = geometry.ib->GetIndexCount();
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