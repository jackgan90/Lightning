#include "ForwardRenderPass.h"
#include "Renderer.h"
#include "FrameMemoryAllocator.h"
#include "ShaderParameter.h"
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
					const auto& unit = renderQueue[i];
					CommitShaderParameters(unit);
					CommitPipelineStates(unit);
					CommitBuffers(unit.geometry);
					Draw(unit.geometry);
				}
			});
		}

		void ForwardRenderPass::OnAddRenderUnit(const RenderUnit& unit)
		{

		}

		void ForwardRenderPass::OnFrameEnd()
		{
		}

		void ForwardRenderPass::CommitPipelineStates(const RenderUnit& unit)
		{
			PipelineState state;
			state.Reset();
			state.renderTargetCount = unit.renderTargetCount;
			auto renderer = Renderer::Instance();
			auto pSwapChain = renderer->GetSwapChain();
			for (auto i = 0;i < unit.renderTargetCount;++i)
			{
				state.renderTargets[i] = unit.renderTargets[i];
			}
			if (unit.material)
			{
				state.vs = unit.material->GetShader(ShaderType::VERTEX);
				state.fs = unit.material->GetShader(ShaderType::FRAGMENT);
				state.gs = unit.material->GetShader(ShaderType::GEOMETRY);
				state.hs = unit.material->GetShader(ShaderType::TESSELATION_CONTROL);
				state.ds = unit.material->GetShader(ShaderType::TESSELATION_EVALUATION);
				for (auto i = 0;i < state.renderTargetCount;++i)
				{
					unit.material->GetBlendState(state.blendStates[i]);
					if (state.blendStates[i].enable)
					{
						state.depthStencilState.depthWriteEnable = false;
					}
				}
			}
			state.primType = unit.geometry.primType;
			//TODO : Apply other pipeline states(blend state, rasterizer state etc)
			
			GetInputLayouts(unit.geometry, state.inputLayouts, state.inputLayoutCount);

			if (unit.depthStencilBuffer)
			{
				state.depthStencilState.bufferFormat = unit.depthStencilBuffer->GetRenderFormat();
			}
			renderer->ApplyRenderTargets(unit.renderTargets, unit.renderTargetCount, unit.depthStencilBuffer);
			renderer->ApplyPipelineState(state);
		}

		void ForwardRenderPass::CommitShaderParameters(const RenderUnit& unit)
		{
			if (!unit.material)
				return;
			static const ShaderType shaderTypes[] = { ShaderType::VERTEX, ShaderType::FRAGMENT, ShaderType::GEOMETRY,
			ShaderType::TESSELATION_CONTROL, ShaderType::TESSELATION_EVALUATION };
			auto renderer = Renderer::Instance();
			for (auto shaderType : shaderTypes)
			{
				IShader* shader = unit.material->GetShader(shaderType);
				if (shader)
				{
					auto parameterCount = unit.material->GetParameterCount(shaderType);
					for (auto i = 0;i < parameterCount;++i)
					{
						auto parameter = unit.material->GetParameter(shaderType, i);
						shader->SetParameter(parameter);
					}
					CommitSemanticUniforms(shader, unit);
				}
			}
		}

		void ForwardRenderPass::CommitSemanticUniforms(IShader* shader, const RenderUnit& unit)
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
					auto wvp = unit.projectionMatrix * unit.viewMatrix * unit.transform.matrix;
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