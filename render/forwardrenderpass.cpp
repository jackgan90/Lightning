#include "forwardrenderpass.h"
#include "renderer.h"

namespace LightningGE
{
	namespace Render
	{
		void ForwardRenderPass::Draw(const RenderItem& item)
		{
			m_renderItems.push_back(item);
		}

		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void ForwardRenderPass::Apply()
		{
			for (auto& renderItem : m_renderItems)
			{
				CommitBuffers(renderItem.geometry);
				CommitPipelineStates(renderItem);
				SetShaderArguments(renderItem);
			}
			m_renderItems.clear();
		}

		void ForwardRenderPass::CommitPipelineStates(const RenderItem& item)
		{
			PipelineState state{};
			if (item.material)
			{
				state.vs = item.material->GetShader(ShaderType::VERTEX);
				state.fs = item.material->GetShader(ShaderType::FRAGMENT);
				state.gs = item.material->GetShader(ShaderType::GEOMETRY);
				state.hs = item.material->GetShader(ShaderType::TESSELATION_CONTROL);
				state.ds = item.material->GetShader(ShaderType::TESSELATION_EVALUATION);
			}
			//TODO : Apply other pipeline states(blend state, rasterizer state etc)
			auto pDevice = Renderer::Instance()->GetDevice();
			state.inputLayouts = GetInputLayouts(item.geometry);
			pDevice->ApplyPipelineState(state);
		}

		void ForwardRenderPass::SetShaderArguments(const RenderItem& item)
		{
			//TODO : set other arguments for shaders
			if (!item.material)
				return;
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
						auto wvp = item.projectionMatrix * item.viewMatrix * item.transform.GetTransformMatrix();
						ShaderArgument arg{ wvp };
						arg.name = "wvp";
						vs->SetArgument(arg);
					}
					break;
				}
				default:
					break;
				}
			}
		}

		std::vector<VertexInputLayout> ForwardRenderPass::GetInputLayouts(const SharedGeometryPtr& geometry)
		{
			std::vector<VertexInputLayout> layouts;
			for (std::size_t i = 0;i < MAX_GEOMETRY_BUFFER_COUNT;i++)
			{
				if (!geometry->vbs[i])
					continue;
				VertexInputLayout layout;
				layout.slot = i;
				for (std::size_t j = 0;j < geometry->vbs[i]->GetComponentCount();++j)
				{
					layout.components.push_back(geometry->vbs[i]->GetComponentInfo(j));
				}
				layouts.push_back(layout);
			}
			return layouts;
		}

		void ForwardRenderPass::CommitBuffers(const SharedGeometryPtr& geometry)
		{
			auto pDevice = Renderer::Instance()->GetDevice();
			for (size_t i = 0; i < MAX_GEOMETRY_BUFFER_COUNT; i++)
			{
				if (!geometry->vbs[i])
					continue;
				if (geometry->vbs_dirty[i])
				{
					pDevice->CommitGPUBuffer(geometry->vbs[i].get());
					geometry->vbs_dirty[i] = false;
				}
			}
			if (geometry->ib && geometry->ib_dirty)
			{
				pDevice->CommitGPUBuffer(geometry->ib.get());
				geometry->ib_dirty = false;
			}
		}
	}
}