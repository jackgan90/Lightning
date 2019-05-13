#include <cassert>
#include "Device.h"
#include "Renderer.h"
#include "FrameMemoryAllocator.h"
#include "DrawCommand.h"
#include "Loader.h"
#include "Serializers/ShaderSerializer.h"
#include "Serializers/TextureSerializer.h"
#include "RenderPass/ForwardRenderPass.h"

namespace Lightning
{
	namespace Render
	{
		IRenderer* Renderer::sInstance{ nullptr };
		FrameMemoryAllocator g_RenderAllocator;
		
		void FrameResource::OnFrameBegin()
		{

		}

		void FrameResource::Release()
		{
			fence.reset();
			defaultDepthStencilBuffer.reset();
		}

		Renderer::Renderer(Window::IWindow* window)
			: mOutputWindow(window)
			, mFrameCount(0)
			, mFrameResourceIndex(0)
			, mStarted(false)
		{
			assert(!sInstance);
			sInstance = this;
			for (const auto& semanticItem : PipelineInputSemantics)
			{
				mPipelineInputSemanticInfos[semanticItem.semantic] = ParsePipelineInputSemantics(semanticItem);
			}
			for (const auto& uniformSemantic : UniformSemantics)
			{
				mUniformToSemantics[uniformSemantic.name] = uniformSemantic.semantic;
				mSemanticsToUniform[uniformSemantic.semantic] = uniformSemantic.name;
			}
		}

		Renderer::~Renderer()
		{
			assert(sInstance == this);
			sInstance = nullptr;
		}

		void Renderer::Render()
		{
			if (!mStarted)
				return;
			WaitForPreviousFrame(false);
			HandleWindowResize();
			mFrameCount++;
			mFrameResources[mFrameResourceIndex].OnFrameBegin();
			OnFrameBegin();
			if (mRootRenderPass)
			{
				mRootRenderPass->BeginRender(*this);
			}
			OnFrameUpdate();
			if (mRootRenderPass)
			{
				mRootRenderPass->Render(*this);
			}
			OnFrameEnd();
			if (mRootRenderPass)
			{
				mRootRenderPass->EndRender(*this);
			}
			mFrameResources[mFrameResourceIndex].frame = mFrameCount;
			mFrameResources[mFrameResourceIndex].fence->SetTargetValue(mFrameCount);
			mSwapChain->Present();
			mFrameResourceIndex = (mFrameResourceIndex + 1) % RENDER_FRAME_COUNT;
			g_RenderAllocator.FinishFrame(mFrameCount);
		}

		IDevice* Renderer::GetDevice()
		{
			return mDevice.get();
		}

		ISwapChain* Renderer::GetSwapChain()
		{
			return mSwapChain.get();
		}

		bool Renderer::CheckIfDepthStencilBufferNeedsResize()
		{
			auto width = mOutputWindow->GetWidth();
			auto height = mOutputWindow->GetHeight();
			auto depthStencilBuffer = GetDefaultDepthStencilBuffer();
			if (!depthStencilBuffer)
				return false;
			auto depthStencilTexture = depthStencilBuffer->GetTexture();
			if (depthStencilTexture->GetWidth() != width || depthStencilTexture->GetHeight() != height)
			{
				return true;
			}
			return false;
		}

		void Renderer::Draw(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera)
		{
			if (mRootRenderPass)
			{
				mRootRenderPass->AddDrawable(drawable, camera);
			}
		}

		void Renderer::GetSemanticInfo(RenderSemantics semantic, SemanticIndex& index, std::string& name)
		{
			auto it = mPipelineInputSemanticInfos.find(semantic);
			assert(it != mPipelineInputSemanticInfos.end() && "Invalid semantic!");
			index = it->second.index;
			name = it->second.name;
		}

		std::uint64_t Renderer::GetCurrentFrameCount()const
		{
			return mFrameCount;
		}

		std::size_t Renderer::GetFrameResourceIndex()const
		{
			return mFrameResourceIndex;
		}

		void Renderer::Start()
		{
			if (mStarted)
				return;
			mDevice.reset(CreateDevice());
			mSwapChain.reset(CreateSwapChain());
			for (size_t i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				mFrameResources[i].fence.reset(CreateRenderFence());
				TextureDescriptor descriptor;
				descriptor.depth = 1;
				descriptor.width = mOutputWindow->GetWidth();
				descriptor.height = mOutputWindow->GetHeight();
				descriptor.multiSampleCount = mSwapChain->GetMultiSampleCount();
				descriptor.multiSampleQuality = mSwapChain->GetMultiSampleQuality();
				descriptor.numberOfMipmaps = 1;
				descriptor.dimension = TEXTURE_DIMENSION_2D;
				descriptor.format = RenderFormat::D24_S8;
				descriptor.depthClearValue = 1.0f;
				descriptor.stencilClearValue = 0;
				auto texture = mDevice->CreateTexture(descriptor, nullptr);
				mFrameResources[i].defaultDepthStencilBuffer = mDevice->CreateDepthStencilBuffer(texture);
			}
			mFrameResourceIndex = 0;
			mRootRenderPass = std::make_unique<ForwardRenderPass>();
			mStarted = true;
		}

		void Renderer::ShutDown()
		{
			if (!mStarted)
				return;
			WaitForPreviousFrame(true);
			for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				mFrameResources[i].Release();
			}
			mDevice.reset();
			mSwapChain.reset();
			mRootRenderPass.reset();
			mStarted = false;
		}

		std::shared_ptr<IDepthStencilBuffer> Renderer::GetDefaultDepthStencilBuffer()
		{
			return mFrameResources[mFrameResourceIndex].defaultDepthStencilBuffer;
		}

		std::shared_ptr<IRenderTarget> Renderer::GetDefaultRenderTarget()
		{
			return mSwapChain->GetCurrentRenderTarget();
		}

		RenderSemantics Renderer::GetUniformSemantic(const char* uniform_name)
		{
			auto it = mUniformToSemantics.find(std::string(uniform_name));
			if (it != mUniformToSemantics.end())
			{
				return it->second;
			}
			return RenderSemantics::UNKNOWN;
		}

		const char* Renderer::GetUniformName(RenderSemantics semantic)
		{
			auto it = mSemanticsToUniform.find(semantic);
			if (it != mSemanticsToUniform.end())
			{
				return it->second;
			}
			return nullptr;
		}

		void Renderer::WaitForPreviousFrame(bool waitAll)
		{
			static std::vector<std::size_t> resourceIndice;
			resourceIndice.clear();
			if (!waitAll)
			{
				resourceIndice.push_back(mFrameResourceIndex);
			}
			else
			{
				for (std::uint32_t i = 0;i < RENDER_FRAME_COUNT;++i)
				{
					resourceIndice.push_back(i);
					//explicit signal to prevent release assert
					mFrameResources[i].fence->SetTargetValue(mFrameResources[i].fence->GetTargetValue() + 1);
				}
			}
			for (auto resourceIndex : resourceIndice)
			{
				auto& frameResource = mFrameResources[resourceIndex];
				frameResource.fence->WaitForTarget();
				g_RenderAllocator.ReleaseFramesBefore(frameResource.frame);
			}
		}

		Renderer::SemanticInfo Renderer::ParsePipelineInputSemantics(const SemanticItem& item)
		{
			SemanticInfo info;
			const std::string fullName(item.name);
			int i = static_cast<int>(fullName.length() - 1);
			std::string indexString;
			for (;i >= 0;--i)
			{
				if (fullName[i] >= '0' && fullName[i] <= '9')
					indexString += fullName[i];
				else
					break;
			}
			assert(indexString.length() < fullName.length() && "Semantic name error!A semantic name cannot be comprised of digits only!");
			if (indexString.length() > 0)
			{
				std::reverse(indexString.begin(), indexString.end());
				info.index = std::stoi(indexString);
			}
			else
			{
				info.index = 0;
			}
			info.rawName = fullName;
			info.name = fullName.substr(0, i + 1);
			return info;
		}

		void Renderer::HandleWindowResize()
		{
			if (CheckIfDepthStencilBufferNeedsResize() || mSwapChain->CheckIfBackBufferNeedsResize())
			{
				//Critical:must wait for previous frame finished.Because previous frames may reference the render target
				//we are about to resize.
				WaitForPreviousFrame(true);
				std::size_t width = mOutputWindow->GetWidth();
				std::size_t height = mOutputWindow->GetHeight();
				//Resize swap chain first.Because width and height may be zero,which cause swap chain
				//resize to a default size.And we base on this size to resize depth stencil buffer.
				mSwapChain->Resize(width, height);
				auto renderTarget = mSwapChain->GetCurrentRenderTarget();
				auto renderTexture = renderTarget->GetTexture();
				width = renderTexture->GetWidth();
				height = renderTexture->GetHeight();
				for (std::uint8_t i = 0;i < RENDER_FRAME_COUNT;++i)
				{
					ResizeDepthStencilBuffer(mFrameResources[i].defaultDepthStencilBuffer.get(), width, height);
				}
			}
		}
	}
}