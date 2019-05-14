#include <cassert>
#include <algorithm>
#include "DrawCommand.h"
#include "DrawCommand.h"
#include "FrameMemoryAllocator.h"
#include "Logger.h"
#undef min
#undef max

namespace Lightning
{
	namespace Render
	{
		extern FrameMemoryAllocator g_RenderAllocator;
		DrawCommand::DrawCommand(IRenderer& renderer, IRenderPass& renderPass)
			: mRenderPass(renderPass), mRenderer(renderer)
		{

		}

		DrawCommand::~DrawCommand()
		{
			DoReset();
		}

		void DrawCommand::SetPrimitiveType(PrimitiveType type)
		{
			mPrimitiveType = type;
		}

		void DrawCommand::SetIndexBuffer(const std::shared_ptr<IIndexBuffer>& indexBuffer)
		{
			mIndexBuffer = indexBuffer;
		}

		void DrawCommand::ClearVertexBuffers()
		{
			DoClearVertexBuffers();
		}

		void DrawCommand::SetVertexBuffers(const std::vector<std::shared_ptr<IVertexBuffer>>& vertexBuffers)
		{
			mVertexBuffers = vertexBuffers;
		}

		void DrawCommand::SetMaterial(const std::shared_ptr<IMaterial>& material)
		{
			mMaterial = material;
		}

		void DrawCommand::SetTransform(const Transform& transform)
		{
			mTransform = transform;
		}

		void DrawCommand::SetViewMatrix(const Matrix4f& matrix)
		{
			mViewMatrix = matrix;
		}

		void DrawCommand::SetProjectionMatrix(const Matrix4f& matrix)
		{
			mProjectionMatrix = matrix;
		}

		void DrawCommand::Reset()
		{
			DoReset();
		}

		void DrawCommand::Commit()
		{
			CommitShaderParameters();
			CommitPipelineStates();
			CommitBuffers();
			Draw();
		}

		void DrawCommand::DoReset()
		{
			mIndexBuffer.reset();
			mMaterial.reset();
			DoClearVertexBuffers();
		}

		void DrawCommand::DoClearVertexBuffers()
		{
			mVertexBuffers.clear();
			//We don't need to deallocate the memories allocated for mVertexBuffers
			//because the memory will be deallocated by Renderer
		}

		void DrawCommand::Release()
		{
			this->~DrawCommand();
			DrawCommandPool::free(this);
		}

		void DrawCommand::CommitShaderParameters()
		{
			static const ShaderType shaderTypes[] =
			{
				ShaderType::VERTEX,
				ShaderType::FRAGMENT,
				ShaderType::GEOMETRY,
				ShaderType::HULL,
				ShaderType::DOMAIN
			};
			if (!mMaterial)
				return;
			for (auto shaderType : shaderTypes)
			{
				auto shader = mMaterial->GetShader(shaderType);
				if (shader)
				{
					mMaterial->VisitParameters([this, &shader](const Parameter& parameter) {
						auto shaderParamType = shader->GetParameterType(parameter.GetName());
						if (shaderParamType != ParameterType::UNKNOWN)
						{
							shader->SetParameter(parameter);
						}
					});
					CommitSemanticUniforms(shader.get());
				}
			}
		}

		void DrawCommand::CommitPipelineStates()
		{
			PipelineState state;
			state.Reset();
			auto renderTargetCount = mRenderPass.GetRenderTargetCount();
			for (auto i = 0;i < renderTargetCount;++i)
			{
				BlendState blendState;
				if (mMaterial)
				{
					mMaterial->GetBlendState(blendState);
					if (blendState.enable)
					{
						state.depthStencilState.depthWriteEnable = false;
					}
				}
				auto renderTarget = mRenderPass.GetRenderTarget(i);
				state.renderTargetBlendStates.push_back({renderTarget, blendState });
			}
			auto depthStencilBuffer = mRenderPass.GetDepthStencilBuffer();
			if (depthStencilBuffer)
			{
				auto depthStencilTexture = depthStencilBuffer->GetTexture();
				state.depthStencilState.bufferFormat = depthStencilTexture->GetRenderFormat();
			}
			//renderer->ApplyRenderTargets(renderTargets, mRenderTargets.size(), depthStencilBuffer.get());
			if (mMaterial)
			{
				state.vs = mMaterial->GetShader(ShaderType::VERTEX);
				state.fs = mMaterial->GetShader(ShaderType::FRAGMENT);
				state.gs = mMaterial->GetShader(ShaderType::GEOMETRY);
				state.hs = mMaterial->GetShader(ShaderType::HULL);
				state.ds = mMaterial->GetShader(ShaderType::DOMAIN);
			}
			state.primType = mPrimitiveType;
			//TODO : Apply other pipeline states(blend state, rasterizer state etc)
			
			GetInputLayouts(state.inputLayouts);

			mRenderer.ApplyPipelineState(state);
		}

		void DrawCommand::CommitBuffers()
		{
			for (std::uint8_t i = 0; i < mVertexBuffers.size(); i++)
			{
				mVertexBuffers[i]->Commit();
				mRenderer.BindVertexBuffer(i, mVertexBuffers[i].get());
			}
			if (mIndexBuffer)
			{
				mIndexBuffer->Commit();
				mRenderer.BindIndexBuffer(mIndexBuffer.get());
			}
		}

		void DrawCommand::Draw()
		{
			if (mIndexBuffer)
			{
				DrawParam param{};
				param.drawType = DrawType::Index;
				param.indexCount = mIndexBuffer->GetIndexCount();
				param.instanceCount = 1;
				mRenderer.Draw(param);
			}
			else
			{
				//TODO : should implement
			}
		}

		void DrawCommand::CommitSemanticUniforms(IShader* shader)
		{
			RenderSemantics* semantics{ nullptr };
			std::uint16_t semanticCount{ 0 };
			shader->GetUniformSemantics(&semantics, semanticCount);
			if (semanticCount == 0)
				return;
			for (auto i = 0;i < semanticCount;++i)
			{
				auto semantic = semantics[i];
				auto uniformName = mRenderer.GetUniformName(semantic);
				switch (semantic)
				{
				case RenderSemantics::WVP:
				{
					//We know that transform.ToMatrix4 may change it's internal matrix
					auto wvp = mProjectionMatrix * mViewMatrix * mTransform.GetMatrix();
					shader->SetParameter(Parameter(uniformName, wvp));
					break;
				}
				default:
					LOG_WARNING("Unsupported semantics : {0}", semantic);
					break;
				}
			}
		}

		void DrawCommand::GetInputLayouts(std::vector<VertexInputLayout>& inputLayouts)
		{
			for (auto i = 0;i < mVertexBuffers.size(); ++i)
			{
				VertexInputLayout layout;
				auto& vertexDescriptor = mVertexBuffers[i]->GetVertexDescriptor();
				layout.slot = i;
				layout.componentCount = vertexDescriptor.componentCount;
				if (layout.componentCount > 0)
				{
					layout.components = g_RenderAllocator.Allocate<VertexComponent>(layout.componentCount);
					std::memcpy(layout.components, vertexDescriptor.components, sizeof(VertexComponent) * layout.componentCount);
				}
				inputLayouts.push_back(layout);
			}
		}
	}
}