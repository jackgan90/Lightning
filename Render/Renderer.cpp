#include <cassert>
#include "Device.h"
#include "Renderer.h"
#include "ForwardRenderPass.h"
#include "DeferedRenderPass.h"
#include "FrameMemoryAllocator.h"
#include "Serializers/ShaderSerializer.h"
#include "Serializers/TextureSerializer.h"
#include "Loader.h"
//for test only
//#include "LoaderPlugin.h"
//#include "Serializers/TextureSerializer.h"
//#include "IPluginManager.h"
//end test

namespace Lightning
{
	/*
	namespace Plugins
	{
		extern IPluginManager* gPluginMgr;
	}*/
	namespace Render
	{
		IRenderer* Renderer::sInstance{ nullptr };
		FrameMemoryAllocator g_RenderAllocator;
		
		void FrameResource::ReleaseRenderQueue()
		{
			if (renderQueue)
			{
				for (auto& node : *renderQueue)
				{
					node.material->Release();
					if (node.geometry.ib)
					{
						node.geometry.ib->Release();
					}
					for (auto i = 0;i < Foundation::ArraySize(node.geometry.vbs);++i)
					{
						if (node.geometry.vbs[i])
						{
							node.geometry.vbs[i]->Release();
						}
					}
					if (node.depthStencilBuffer)
						node.depthStencilBuffer->Release();
					for (auto renderTarget : node.renderTargets)
					{
						renderTarget->Release();
					}
				}
				renderQueue->clear();
			}
		}

		void FrameResource::OnFrameBegin()
		{
			ReleaseRenderQueue();
		}

		void FrameResource::Release()
		{
			if (fence)
			{
				delete fence;
				fence = nullptr;
			}
			if (defaultDepthStencilBuffer)
				defaultDepthStencilBuffer->Release();
			ReleaseRenderQueue();
		}

		Renderer::Renderer(Window::IWindow* window)
			: mOutputWindow(window)
			, mFrameCount(0)
			, mFrameResourceIndex(0)
			, mClearColor(0.5f, 0.5f, 0.5f, 1.0f)
			, mRenderQueueIndex(RENDER_FRAME_COUNT)
			, mCurrentFrameRenderQueue(&mRenderQueues[RENDER_FRAME_COUNT])
			, mStarted(false)
		{
			assert(!sInstance);
			mOutputWindow->AddRef();
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

		Renderer::SemanticInfo Renderer::ParsePipelineInputSemantics(const PipelineInputSemantic& item)
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
			info.name = fullName.substr(0, i + 1);
			return info;
		}

		void Renderer::GetSemanticInfo(RenderSemantics semantic, SemanticIndex& index, std::string& name)
		{
			auto it = mPipelineInputSemanticInfos.find(semantic);
			assert(it != mPipelineInputSemanticInfos.end() && "Invalid semantic!");
			index = it->second.index;
			name = it->second.name;
		}

		void Renderer::ApplyRenderPasses()
		{
			for (auto& pass : mRenderPasses)
			{
				pass->Apply(*mFrameResources[mFrameResourceIndex].renderQueue);
			}
		}

		void Renderer::Render()
		{
			if (!mStarted)
				return;
			WaitForPreviousFrame(false);
			mFrameCount++;
			mFrameResources[mFrameResourceIndex].OnFrameBegin();
			mFrameResources[mFrameResourceIndex].renderQueue = mCurrentFrameRenderQueue;
			if (mRenderQueueIndex == RENDER_FRAME_COUNT)
			{
				mRenderQueueIndex = 0;
			}
			else
			{
				++mRenderQueueIndex;
			}

			mCurrentFrameRenderQueue = &mRenderQueues[mRenderQueueIndex];
			OnFrameBegin();
			InvokeEventCallback(RendererEvent::FRAME_BEGIN);
			auto defaultRenderTarget = mSwapChain->GetDefaultRenderTarget();
			ClearRenderTarget(defaultRenderTarget, mClearColor);
			auto depthStencilBuffer = GetDefaultDepthStencilBuffer();
			ClearDepthStencilBuffer(depthStencilBuffer, DepthStencilClearFlags::CLEAR_DEPTH | DepthStencilClearFlags::CLEAR_STENCIL,
				depthStencilBuffer->GetDepthClearValue(), depthStencilBuffer->GetStencilClearValue(), nullptr);
			OnFrameUpdate();
			InvokeEventCallback(RendererEvent::FRAME_UPDATE);
			ApplyRenderPasses();
			OnFrameEnd();
			InvokeEventCallback(RendererEvent::FRAME_END);
			auto fence = mFrameResources[mFrameResourceIndex].fence;
			mFrameResources[mFrameResourceIndex].frame = mFrameCount;
			fence->SetTargetValue(mFrameCount);
			mSwapChain->Present();
			mFrameResourceIndex = mSwapChain->GetCurrentBackBufferIndex();
			g_RenderAllocator.FinishFrame(mFrameCount);
		}

		void Renderer::OnFrameBegin()
		{
		}

		//ITexture* testTex{ nullptr };
		void Renderer::OnFrameUpdate()
		{
			/*
			if (testTex)
				testTex->Commit();
			static bool textureLoaded{ false };
			if (!textureLoaded)
			{
				textureLoaded = true;
				auto loader = Plugins::gPluginMgr->GetPlugin<Plugins::LoaderPlugin>("Loader")->GetLoader();
				auto ser = new TextureSerializer("Purged_One_Human_Jumper.jpg", [this](ITexture* texture) {
					if (texture)
					{
						//texture->Commit();
						testTex = texture;
					}
				});
				loader->Load("Purged_One_Human_Jumper.jpg", ser);
			}
			*/
		}

		void Renderer::OnFrameEnd()
		{

		}


		void Renderer::SetClearColor(const ColorF& color)
		{
			mClearColor = color;
		}

		std::uint64_t Renderer::GetCurrentFrameCount()const
		{
			return mFrameCount;
		}

		IDevice* Renderer::GetDevice()
		{
			return mDevice.get();
		}

		ISwapChain* Renderer::GetSwapChain()
		{
			return mSwapChain.get();
		}


		void Renderer::CommitRenderNode(const RenderNode& node)
		{
			assert(mStarted && "Renderer must be started first.");
			assert(node.material && "node must have a material!");
			node.material->AddRef();
			bool hasIB = false;
			bool hasVB = false;
			if (node.geometry.ib)
			{
				hasIB = true;
				node.geometry.ib->AddRef();
			}
			for (auto i = 0;i < Foundation::ArraySize(node.geometry.vbs);++i)
			{
				if (node.geometry.vbs[i])
				{
					node.geometry.vbs[i]->AddRef();
					hasVB = true;
				}

			}
			assert((hasVB || hasIB) && "vb or ib can not both be empty!");
			if (node.depthStencilBuffer)
				node.depthStencilBuffer->AddRef();
			for (auto renderTarget : node.renderTargets)
			{
				assert(renderTarget && "renderTarget cannot be null!");
				renderTarget->AddRef();
			}
			mCurrentFrameRenderQueue->push_back(node);
			for (auto& pass : mRenderPasses)
			{
				pass->OnAddRenderNode(node);
			}
		}

		void Renderer::AddRenderPass(RenderPassType type)
		{
			auto pass = CreateRenderPass(type);
			if(pass)
				mRenderPasses.emplace_back(pass);
		}

		std::size_t Renderer::GetFrameResourceIndex()const
		{
			return mFrameResourceIndex;
		}

		void Renderer::RegisterCallback(RendererEvent evt, RendererCallback cb)
		{
			mCallbacks[evt].push_back(cb);
		}

		RenderPass* Renderer::CreateRenderPass(RenderPassType type)
		{
			switch (type)
			{
			case RenderPassType::FORWARD:
				return new ForwardRenderPass();
			case RenderPassType::DEFERED:
				return new DeferedRenderPass();
			default:
				break;
			}
			return nullptr;
		}

		void Renderer::Start()
		{
			if (mStarted)
				return;
			mDevice.reset(CreateDevice());
			mSwapChain.reset(CreateSwapChain());
			for (size_t i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				mFrameResources[i].fence = CreateRenderFence();
				mFrameResources[i].defaultDepthStencilBuffer = 
					CreateDepthStencilBuffer( mOutputWindow->GetWidth(), mOutputWindow->GetHeight());
			}
			mFrameResourceIndex = mSwapChain->GetCurrentBackBufferIndex();
			AddRenderPass(RenderPassType::FORWARD);
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
			mFrameResources[0].renderQueue = mCurrentFrameRenderQueue;
			mFrameResources[0].ReleaseRenderQueue();
			mOutputWindow->Release();
			mDevice.reset();
			mSwapChain.reset();
			mRenderPasses.clear();
			mCallbacks.clear();
			mStarted = false;
		}

		IDepthStencilBuffer* Renderer::GetDefaultDepthStencilBuffer()
		{
			return mFrameResources[mFrameResourceIndex].defaultDepthStencilBuffer;
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
			Container::Vector<UINT> bufferIndice;
			if (!waitAll)
			{
				bufferIndice.push_back(mSwapChain->GetCurrentBackBufferIndex());
			}
			else
			{
				for (std::uint32_t i = 0;i < RENDER_FRAME_COUNT;++i)
				{
					bufferIndice.push_back(i);
					//explicit signal to prevent release assert
					mFrameResources[i].fence->SetTargetValue(mFrameResources[i].fence->GetTargetValue() + 1);
				}
			}
			for (const auto& bufferIndex : bufferIndice)
			{
				auto& frameResource = mFrameResources[bufferIndex];
				frameResource.fence->WaitForTarget();
				g_RenderAllocator.ReleaseFramesBefore(frameResource.frame);
			}
		}

		void Renderer::InvokeEventCallback(RendererEvent evt)
		{
			auto it = mCallbacks.find(evt);
			if (it != mCallbacks.end())
			{
				for (auto& func : it->second)
					func();
			}
		}
	}
}