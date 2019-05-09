#include "CommitedDrawCall.h"
#include "FrameMemoryAllocator.h"
#include "ShaderParameter.h"
#include "Renderer.h"
#include "Logger.h"

namespace Lightning
{
	namespace Render
	{
		extern FrameMemoryAllocator g_RenderAllocator;
		CommitedDrawCall::CommitedDrawCall()
		{

		}

		CommitedDrawCall::~CommitedDrawCall()
		{
			mIndexBuffer.reset();
			for (auto i = 0;i < mVertexBufferCount;++i)
			{
				mVertexBuffers[i].vertexBuffer.reset();
			}
		}

		PrimitiveType CommitedDrawCall::GetPrimitiveType()const
		{
			return mPrimitiveType;
		}

		std::shared_ptr<IIndexBuffer> CommitedDrawCall::GetIndexBuffer()const
		{
			return mIndexBuffer;
		}

		std::size_t CommitedDrawCall::GetVertexBufferCount()const
		{
			return mVertexBufferCount;
		}

		void CommitedDrawCall::GetVertexBuffer(std::size_t index, std::size_t& slot, std::shared_ptr<IVertexBuffer>& vertexBuffer)const
		{
			slot = mVertexBuffers[index].slot;
			vertexBuffer = mVertexBuffers[index].vertexBuffer;
		}

		std::shared_ptr<IMaterial> CommitedDrawCall::GetMaterial()const
		{
			return mMaterial;
		}

		const Transform& CommitedDrawCall::GetTransform()const
		{
			return mTransform;
		}

		const Matrix4f& CommitedDrawCall::GetViewMatrix()const
		{
			return mViewMatrix;
		}

		const Matrix4f& CommitedDrawCall::GetProjectionMatrix()const
		{
			return mProjectionMatrix;
		}

		std::shared_ptr<IRenderTarget> CommitedDrawCall::GetRenderTarget(std::size_t index)const
		{
			return mRenderTargets[index];
		}

		std::size_t CommitedDrawCall::GetRenderTargetCount()const
		{
			return mRenderTargets.size();
		}

		std::shared_ptr<IDepthStencilBuffer> CommitedDrawCall::GetDepthStencilBuffer()const
		{
			return mDepthStencilBuffer;
		}

		std::size_t CommitedDrawCall::GetViewportCount()const
		{
			return mViewportCount;
		}

		void CommitedDrawCall::GetViewportAndScissorRect(std::size_t index, Viewport& viewport, ScissorRect& scissorRect)const
		{
			viewport = mViewportAndScissorRects[index].viewport;
			scissorRect = mViewportAndScissorRects[index].scissorRect;
		}

		void CommitedDrawCall::Release()
		{
			this->~CommitedDrawCall();
			CommitedDrawCallPool::free(this);
		}

		void CommitedDrawCall::Apply()
		{
			CommitShaderParameters();
			CommitPipelineStates();
			CommitBuffers();
			Draw();
		}

		void CommitedDrawCall::CommitShaderParameters()
		{
			auto material = GetMaterial();
			if (!material)
				return;
			static const ShaderType shaderTypes[] = { ShaderType::VERTEX, ShaderType::FRAGMENT, ShaderType::GEOMETRY,
			ShaderType::HULL, ShaderType::DOMAIN };
			auto renderer = Renderer::Instance();
			for (auto shaderType : shaderTypes)
			{
				auto shader = material->GetShader(shaderType);
				if (shader)
				{
					auto parameterCount = material->GetParameterCount(shaderType);
					for (auto i = 0;i < parameterCount;++i)
					{
						auto parameter = material->GetParameter(shaderType, i);
						if (parameter)
						{
							shader->SetParameter(*parameter);
						}
					}
					CommitSemanticUniforms(shader.get());
				}
			}
		}

		void CommitedDrawCall::CommitPipelineStates()
		{
			PipelineState state;
			state.Reset();
			auto renderer = Renderer::Instance();
			auto renderTargetCount = mRenderTargets.size();
			auto renderTargets = g_RenderAllocator.Allocate<IRenderTarget*>(renderTargetCount);
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
				state.renderTargetBlendStates.push_back({ mRenderTargets[i], blendState });
				renderTargets[i] = mRenderTargets[i].get();
			}
			auto depthStencilBuffer = GetDepthStencilBuffer();
			if (depthStencilBuffer)
			{
				auto depthStencilTexture = depthStencilBuffer->GetTexture();
				state.depthStencilState.bufferFormat = depthStencilTexture->GetRenderFormat();
			}
			renderer->ApplyRenderTargets(renderTargets, mRenderTargets.size(), depthStencilBuffer.get());
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
			auto viewportCount = GetViewportCount();
			auto viewports = g_RenderAllocator.Allocate<Viewport>(viewportCount);
			auto scissorRects = g_RenderAllocator.Allocate<ScissorRect>(viewportCount);
			for (auto i = 0;i < viewportCount;++i)
			{
				GetViewportAndScissorRect(i, viewports[i], scissorRects[i]);
			}
			renderer->ApplyViewports(viewports, viewportCount);
			renderer->ApplyScissorRects(scissorRects, viewportCount);
		}

		void CommitedDrawCall::CommitBuffers()
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

		void CommitedDrawCall::Draw()
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

		void CommitedDrawCall::CommitSemanticUniforms(IShader* shader)
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
					shader->SetParameter(ShaderParameter(uniformName, wvp));
					break;
				}
				default:
					LOG_WARNING("Unsupported semantics : {0}", semantic);
					break;
				}
			}
		}

		void CommitedDrawCall::GetInputLayouts(std::vector<VertexInputLayout>& inputLayouts)
		{
			for (std::size_t i = 0;i < mVertexBufferCount;i++)
			{
				std::shared_ptr<IVertexBuffer> vertexBuffer;
				std::size_t slot;
				GetVertexBuffer(i, slot, vertexBuffer);
				VertexInputLayout layout;
				auto& vertexDescriptor = vertexBuffer->GetVertexDescriptor();
				layout.slot = slot;
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