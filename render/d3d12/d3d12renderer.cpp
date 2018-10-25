#include "d3d12renderer.h"
#include "d3d12swapchain.h"
#include "d3d12device.h"
#include "d3d12descriptorheapmanager.h"
#include "d3d12rendertarget.h"
#include "d3d12renderfence.h"
#include "d3d12constantbuffermanager.h"
#include "winwindow.h"
#include "configmanager.h"
#include "logger.h"
#include "common.h"
#include "framememoryallocator.h"
#include "d3d12statefulresourcemgr.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		using WindowSystem::WinWindow;
		extern Foundation::FrameMemoryAllocator g_RenderAllocator;


		D3D12Renderer::~D3D12Renderer()
		{
			//Note:we should release resources in advance to make REPORT_LIVE_OBJECTS work correctly because if we let the share pointer
			//destructor run out of the scope,we cannot trace the objects 
			//device , swap chain and depth stencil buffer are parent class's members but we still release them here because we need to track alive resources
		}

		D3D12Renderer::D3D12Renderer(const SharedWindowPtr& pWindow, const SharedFileSystemPtr& fs) : Renderer(fs, pWindow)
		{
#ifndef NDEBUG
			EnableDebugLayer();
			HRESULT hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&mDXGIFactory));
#else
			HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory));
#endif
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create DXGI factory!");
			}
			LOG_INFO("Initialize D3D12 render context succeeded!");
		}

#ifndef NDEBUG
		void D3D12Renderer::EnableDebugLayer()
		{
			auto res = ::D3D12GetDebugInterface(IID_PPV_ARGS(&mD3D12Debug));
			if (FAILED(res))
			{
				LOG_WARNING("Failed to get d3d12 debug interface!You should enable Graphics Tools optional feature!ErrorCode : {0 : x}", res);
			}
			else
			{
				mD3D12Debug->EnableDebugLayer();
			}
		}
#endif
		void D3D12Renderer::Start()
		{
			Renderer::Start();
#ifndef NDEBUG
			InitDXGIDebug();
#endif
			REPORT_LIVE_OBJECTS;
		}

		void D3D12Renderer::ShutDown()
		{
			LOG_INFO("Start to clean up render resources.");
			Renderer::ShutDown();
			D3D12RenderTargetManager::Instance()->Clear();
			D3D12DescriptorHeapManager::Instance()->Clear();
			D3D12ConstantBufferManager::Instance()->Clear();
			D3D12StatefulResourceMgr::Instance()->Clear();
			mDXGIFactory.Reset();
			REPORT_LIVE_OBJECTS;
			LOG_INFO("Finished reporting live objects!");
		}

		ID3D12CommandQueue* D3D12Renderer::GetCommandQueue()
		{
			return mCommandQueue.Get();
		}

		ID3D12GraphicsCommandList* D3D12Renderer::GetGraphicsCommandList()
		{
			return mCmdEncoders[mFrameResourceIndex]->GetCommandList();
		}

		IRenderFence* D3D12Renderer::CreateRenderFence()
		{
			return new D3D12RenderFence(static_cast<D3D12Device*>(mDevice.get()), 0);
		}

		IDevice* D3D12Renderer::CreateDevice()
		{
			auto device = new D3D12Device(mDXGIFactory.Get(), mFs);
			D3D12_COMMAND_QUEUE_DESC desc = {};
			mCommandQueue = device->CreateCommandQueue(&desc);
			return device;
		}

		void D3D12Renderer::ClearRenderTarget(const SharedRenderTargetPtr& rt, const ColorF& color, const RectIList* rects)
		{
			D3D12RenderTarget *pTarget = static_cast<D3D12RenderTarget*>(rt.get());
			assert(pTarget);
			//should check the type of the rt to transit it from previous state to render target state
			//currently just check back buffer render target
			if (rt->IsSwapChainRenderTarget())
			{
				pTarget->TransitToRTState(mFrontCmdEncoder[mFrameResourceIndex].GetCommandList());
			}

			//cache render target to prevent it from being released before GPU execute ClearRenderTargetView

			auto commandList = GetGraphicsCommandList();
			const float clearColor[] = { color.r, color.g, color.b, color.a };
			auto rtvHandle = pTarget->GetCPUHandle();
			if (rects && !rects->empty())
			{
				//TODO : This implementation is wrong.Should allocate frame memory not local memory,must fix later
				D3D12_RECT* d3dRect = g_RenderAllocator.Allocate<D3D12_RECT>(rects->size());
				for (size_t i = 0; i < rects->size(); i++)
				{
					d3dRect[i].left = (*rects)[i].left;
					d3dRect[i].right = (*rects)[i].right();
					d3dRect[i].top = (*rects)[i].top;
					d3dRect[i].bottom = (*rects)[i].bottom();
				}
				commandList->ClearRenderTargetView(rtvHandle, clearColor, rects->size(), d3dRect);
			}
			else
			{
				commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
			}
		}

		void D3D12Renderer::ClearDepthStencilBuffer(const SharedDepthStencilBufferPtr& buffer, DepthStencilClearFlags flags, 
			float depth, std::uint8_t stencil, const RectIList* rects)
		{
			D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH;
			if ((flags & DepthStencilClearFlags::CLEAR_DEPTH) != DepthStencilClearFlags::CLEAR_DEPTH)
			{
				clearFlags &= ~D3D12_CLEAR_FLAG_DEPTH;
			}
			if ((flags & DepthStencilClearFlags::CLEAR_STENCIL) == DepthStencilClearFlags::CLEAR_STENCIL)
			{
				clearFlags |= D3D12_CLEAR_FLAG_STENCIL;
			}
			auto dsvHandle = static_cast<D3D12DepthStencilBuffer*>(buffer.get())->GetCPUHandle();
			auto commandList = GetGraphicsCommandList();
			if (rects && !rects->empty())
			{
				//TODO : This implementation is wrong.Should allocate frame memory not local memory,must fix later
				D3D12_RECT* d3dRect = g_RenderAllocator.Allocate<D3D12_RECT>(rects->size());
				for (size_t i = 0; i < rects->size(); i++)
				{
					d3dRect[i].left = (*rects)[i].left;
					d3dRect[i].right = (*rects)[i].right();
					d3dRect[i].top = (*rects)[i].top;
					d3dRect[i].bottom = (*rects)[i].bottom();
				}
				commandList->ClearDepthStencilView(dsvHandle, clearFlags, depth, stencil, rects->size(), d3dRect);
			}
			else
			{
				commandList->ClearDepthStencilView(dsvHandle, clearFlags, depth, stencil, 0, nullptr);
			}
		}

		ISwapChain* D3D12Renderer::CreateSwapChain()
		{
			return new D3D12SwapChain(mDXGIFactory.Get(), GetCommandQueue(), mOutputWindow.get());
		}

		IDepthStencilBuffer* D3D12Renderer::CreateDepthStencilBuffer(std::size_t width, std::size_t height)
		{
			return new D3D12DepthStencilBuffer(width, height);
		}

#ifndef NDEBUG
		void D3D12Renderer::InitDXGIDebug()
		{
			//HMODULE dxgiDebugHandle = ::GetModuleHandle("Dxgidebug.dll");
			//don't use GetModuleHandle because Dxgidebug.dll may not be loaded automatically by the app
			//so we just load the dll here explicitly
			HMODULE dxgiDebugHandle = ::LoadLibrary("Dxgidebug.dll");
			if (!dxgiDebugHandle)
			{
				LOG_WARNING("Can't get dxgidebug.dll module,errorCode:{0 : x}", ::GetLastError());
				return;
			}
			//the __stdcall declaration is required because windows APIs conform to stdcall convention
			//omit it will cause stack corruption
			using DXGIGetDebugInterfaceFunc = LRESULT (__stdcall *)(REFIID, void**);
			DXGIGetDebugInterfaceFunc pDXGIGetDebugInterface = reinterpret_cast<DXGIGetDebugInterfaceFunc>(::GetProcAddress(dxgiDebugHandle, "DXGIGetDebugInterface"));
			if (!pDXGIGetDebugInterface)
			{
				LOG_WARNING("Failed to get debug interface!");
				return;
			}
			pDXGIGetDebugInterface(IID_PPV_ARGS(&mDXGIDebug));
		}
#endif

		void D3D12Renderer::BeginFrame()
		{
			Renderer::BeginFrame();
			mCmdEncoders[mFrameResourceIndex].for_each([](D3D12CommandEncoder& encoder) {
				encoder.Reset();
			});
			mFrontCmdEncoder[mFrameResourceIndex].Reset();
			mBackCmdEncoder[mFrameResourceIndex].Reset();
		}

		void D3D12Renderer::DoFrame()
		{
			Renderer::DoFrame();
		}

		void D3D12Renderer::EndFrame()
		{
			auto defaultRenderTarget = mSwapChain->GetDefaultRenderTarget();
			auto renderTarget = static_cast<D3D12RenderTarget*>(defaultRenderTarget.get());
			container::vector<ID3D12CommandList*> commandLists{
				mFrontCmdEncoder[mFrameResourceIndex].GetCommandList(),
			};
			mCmdEncoders[mFrameResourceIndex].for_each([&commandLists](D3D12CommandEncoder& encoder) {
				commandLists.push_back(encoder.GetCommandList());
			});
			commandLists.push_back(mBackCmdEncoder[mFrameResourceIndex].GetCommandList());

			D3D12RenderTargetManager::Instance()->Synchronize();
			renderTarget->TransitToPresentState(mBackCmdEncoder[mFrameResourceIndex].GetCommandList());

			for (auto cmdList : commandLists)
			{
				static_cast<ID3D12GraphicsCommandList*>(cmdList)->Close();
			}
			auto commandQueue = GetCommandQueue();
			D3D12StatefulResourceMgr::Instance()->FixResourceStates(commandLists);
			commandQueue->ExecuteCommandLists(commandLists.size(), &commandLists[0]);
			Renderer::EndFrame();
		}



	}
}