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
					const auto unit = renderQueue[i];
					CommitShaderParameters(unit);
					CommitPipelineStates(unit);
					CommitBuffers(unit);
					Draw(unit);
				}
			});
		}

		void ForwardRenderPass::OnAddRenderUnit(const IImmutableRenderUnit* unit)
		{

		}

		void ForwardRenderPass::OnFrameEnd()
		{
		}

		void ForwardRenderPass::CommitPipelineStates(const IImmutableRenderUnit* unit)
		{
			PipelineState state;
			state.Reset();
			state.renderTargetCount = unit->GetRenderTargetCount();
			auto renderer = Renderer::Instance();
			auto pSwapChain = renderer->GetSwapChain();
			state.renderTargets = g_RenderAllocator.Allocate<IRenderTarget*>(state.renderTargetCount);
			for (auto i = 0;i < state.renderTargetCount;++i)
			{
				state.renderTargets[i] = unit->GetRenderTarget(i);
			}
			auto material = unit->GetMaterial();
			if (material)
			{
				state.vs = material->GetShader(ShaderType::VERTEX);
				state.fs = material->GetShader(ShaderType::FRAGMENT);
				state.gs = material->GetShader(ShaderType::GEOMETRY);
				state.hs = material->GetShader(ShaderType::TESSELATION_CONTROL);
				state.ds = material->GetShader(ShaderType::TESSELATION_EVALUATION);
				state.blendStates = g_RenderAllocator.Allocate<BlendState>(state.renderTargetCount);
				for (auto i = 0;i < state.renderTargetCount;++i)
				{
					material->GetBlendState(state.blendStates[i]);
					if (state.blendStates[i].enable)
					{
						state.depthStencilState.depthWriteEnable = false;
					}
				}
			}
			state.primType = unit->GetPrimitiveType();
			//TODO : Apply other pipeline states(blend state, rasterizer state etc)
			
			GetInputLayouts(unit, state.inputLayouts, state.inputLayoutCount);

			auto depthStencilBuffer = unit->GetDepthStencilBuffer();
			if (depthStencilBuffer)
			{
				auto depthStencilTexture = depthStencilBuffer->GetTexture();
				state.depthStencilState.bufferFormat = depthStencilTexture->GetRenderFormat();
			}
			renderer->ApplyRenderTargets(state.renderTargets, state.renderTargetCount, depthStencilBuffer);
			renderer->ApplyPipelineState(state);
			auto viewportCount = unit->GetViewportCount();
			auto viewports = g_RenderAllocator.Allocate<Viewport>(viewportCount);
			auto scissorRects = g_RenderAllocator.Allocate<ScissorRect>(viewportCount);
			for (auto i = 0;i < viewportCount;++i)
			{
				unit->GetViewportAndScissorRect(i, viewports[i], scissorRects[i]);
			}
			renderer->ApplyViewports(viewports, viewportCount);
			renderer->ApplyScissorRects(scissorRects, viewportCount);
		}

		void ForwardRenderPass::CommitShaderParameters(const IImmutableRenderUnit* unit)
		{
			auto material = unit->GetMaterial();
			if (!material)
				return;
			static const ShaderType shaderTypes[] = { ShaderType::VERTEX, ShaderType::FRAGMENT, ShaderType::GEOMETRY,
			ShaderType::TESSELATION_CONTROL, ShaderType::TESSELATION_EVALUATION };
			auto renderer = Renderer::Instance();
			for (auto shaderType : shaderTypes)
			{
				IShader* shader = material->GetShader(shaderType);
				if (shader)
				{
					auto parameterCount = material->GetParameterCount(shaderType);
					for (auto i = 0;i < parameterCount;++i)
					{
						auto parameter = material->GetParameter(shaderType, i);
						shader->SetParameter(parameter);
					}
					CommitSemanticUniforms(shader, unit);
				}
			}
		}

		void ForwardRenderPass::CommitSemanticUniforms(IShader* shader, const IImmutableRenderUnit* unit)
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
					auto wvp = unit->GetProjectionMatrix() * unit->GetViewMatrix() * unit->GetTransform().matrix;
					shader->SetParameter(&ShaderParameter(uniformName, wvp));
					break;
				}
				default:
					LOG_WARNING("Unsupported semantics : {0}", semantic);
					break;
				}
			}
		}

		void ForwardRenderPass::GetInputLayouts(const IImmutableRenderUnit* unit, VertexInputLayout*& layouts, std::size_t& layoutCount)
		{
			layoutCount = unit->GetVertexBufferCount();
			layouts = g_RenderAllocator.Allocate<VertexInputLayout>(layoutCount);
			for (std::size_t i = 0;i < layoutCount;i++)
			{
				IVertexBuffer* vertexBuffer;
				std::size_t slot;
				unit->GetVertexBuffer(i, slot, vertexBuffer);
				auto& layout = layouts[i];
				layout.slot = slot;
				auto& vertexDescriptor = vertexBuffer->GetVertexDescriptor();
				layout.componentCount = vertexDescriptor.componentCount;
				if (layout.componentCount > 0)
				{
					layout.components = g_RenderAllocator.Allocate<VertexComponent>(layout.componentCount);
					std::memcpy(layout.components, vertexDescriptor.components, sizeof(VertexComponent) * layout.componentCount);
				}
			}
		}

		void ForwardRenderPass::CommitBuffers(const IImmutableRenderUnit* unit)
		{
			auto renderer = Renderer::Instance();
			auto vertexBufferCount = unit->GetVertexBufferCount();
			for (std::uint8_t i = 0; i < vertexBufferCount; i++)
			{
				std::size_t slot;
				IVertexBuffer* vertexBuffer;
				unit->GetVertexBuffer(i, slot, vertexBuffer);
				vertexBuffer->Commit();
				renderer->BindGPUBuffer(slot, vertexBuffer);
			}
			auto indexBuffer = unit->GetIndexBuffer();
			if (indexBuffer)
			{
				indexBuffer->Commit();
				renderer->BindGPUBuffer(0, indexBuffer);
			}
		}

		void ForwardRenderPass::Draw(const IImmutableRenderUnit* unit)
		{
			auto renderer = Renderer::Instance();
			auto indexBuffer = unit->GetIndexBuffer();
			if (indexBuffer)
			{
				DrawParam param{};
				param.drawType = DrawType::Index;
				param.indexCount = indexBuffer->GetIndexCount();
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