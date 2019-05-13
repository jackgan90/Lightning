#include <cassert>
#include <algorithm>
#include "Renderer.h"
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
		DrawCommand::DrawCommand(IRenderPass& renderPass)
			: mRenderPass(renderPass)
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

		PrimitiveType DrawCommand::GetPrimitiveType()const
		{
			return mPrimitiveType;
		}

		void DrawCommand::SetIndexBuffer(const std::shared_ptr<IIndexBuffer>& indexBuffer)
		{
			mIndexBuffer = indexBuffer;
		}

		std::shared_ptr<IIndexBuffer> DrawCommand::GetIndexBuffer()const
		{
			return mIndexBuffer;
		}

		void DrawCommand::ClearVertexBuffers()
		{
			DoClearVertexBuffers();
		}

		void DrawCommand::SetVertexBuffer(std::size_t slot, const std::shared_ptr<IVertexBuffer>& vertexBuffer)
		{
			auto it = mVertexBuffers.find(slot);
			if (it != mVertexBuffers.end())
			{
				if (it->second == vertexBuffer)
					return;
				if (vertexBuffer)
				{
					it->second = vertexBuffer;
				}
				else
				{
					mVertexBuffers.erase(it);
				}
			}
			else
			{
				if (vertexBuffer)
				{
					mVertexBuffers.emplace(slot, vertexBuffer);
				}
			}
		}

		std::size_t DrawCommand::GetVertexBufferCount()const
		{
			return mVertexBuffers.size();
		}

		void DrawCommand::GetVertexBuffer(std::size_t index, std::size_t& slot, std::shared_ptr<IVertexBuffer>& vertexBuffer)const
		{
			assert(index < mVertexBuffers.size() && "index out of range.");
			auto i = 0;
			for (auto it = mVertexBuffers.begin(); it != mVertexBuffers.end();++it, ++i)
			{
				if (i == index)
				{
					slot = it->first;
					vertexBuffer = it->second;
					break;
				}
			}
		}

		void DrawCommand::SetMaterial(const std::shared_ptr<IMaterial>& material)
		{
			if (material == mMaterial)
				return;
			mMaterial = material;
		}

		std::shared_ptr<IMaterial> DrawCommand::GetMaterial()const
		{
			return mMaterial;
		}

		void DrawCommand::SetTransform(const Transform& transform)
		{
			mTransform = transform;
		}

		const Transform& DrawCommand::GetTransform()const
		{
			return mTransform;
		}

		void DrawCommand::SetViewMatrix(const Matrix4f& matrix)
		{
			mViewMatrix = matrix;
		}

		const Matrix4f& DrawCommand::GetViewMatrix()const
		{
			return mViewMatrix;
		}

		void DrawCommand::SetProjectionMatrix(const Matrix4f& matrix)
		{
			mProjectionMatrix = matrix;
		}

		const Matrix4f& DrawCommand::GetProjectionMatrix()const
		{
			return mProjectionMatrix;
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
			auto material = GetMaterial();
			if (!material)
				return;
			auto renderer = Renderer::Instance();
			for (auto shaderType : shaderTypes)
			{
				auto shader = material->GetShader(shaderType);
				if (shader)
				{
					material->VisitParameters([&material, &shader](const Parameter& parameter) {
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
			auto renderer = Renderer::Instance();
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
			state.primType = GetPrimitiveType();
			//TODO : Apply other pipeline states(blend state, rasterizer state etc)
			
			GetInputLayouts(state.inputLayouts);

			renderer->ApplyPipelineState(state);
			/*
			auto viewportCount = GetViewportCount();
			auto viewports = g_RenderAllocator.Allocate<Viewport>(viewportCount);
			auto scissorRects = g_RenderAllocator.Allocate<ScissorRect>(viewportCount);
			for (auto i = 0;i < viewportCount;++i)
			{
				GetViewportAndScissorRect(i, viewports[i], scissorRects[i]);
			}
			renderer->ApplyViewports(viewports, viewportCount);
			renderer->ApplyScissorRects(scissorRects, viewportCount);*/
		}

		void DrawCommand::CommitBuffers()
		{
			auto renderer = Renderer::Instance();
			auto vertexBufferCount = GetVertexBufferCount();
			for (std::uint8_t i = 0; i < vertexBufferCount; i++)
			{
				std::size_t slot;
				std::shared_ptr<IVertexBuffer> vertexBuffer;
				GetVertexBuffer(i, slot, vertexBuffer);
				vertexBuffer->Commit();
				renderer->BindVertexBuffer(slot, vertexBuffer.get());
			}
			auto indexBuffer = GetIndexBuffer();
			if (indexBuffer)
			{
				indexBuffer->Commit();
				renderer->BindIndexBuffer(indexBuffer.get());
			}
		}

		void DrawCommand::Draw()
		{
			auto renderer = Renderer::Instance();
			auto indexBuffer = GetIndexBuffer();
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

		void DrawCommand::CommitSemanticUniforms(IShader* shader)
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
					auto wvp = GetProjectionMatrix() * GetViewMatrix() * mTransform.GetMatrix();
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
			for (auto it = mVertexBuffers.begin(); it != mVertexBuffers.end();++it)
			{
				VertexInputLayout layout;
				auto& vertexDescriptor = it->second->GetVertexDescriptor();
				layout.slot = it->first;
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