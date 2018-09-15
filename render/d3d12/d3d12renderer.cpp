#include "d3d12renderer.h"
#include "d3d12swapchain.h"
#include "d3d12device.h"
#include "d3d12descriptorheapmanager.h"
#include "d3d12rendertarget.h"
#include "d3d12renderfence.h"
#include "winwindow.h"
#include "configmanager.h"
#include "logger.h"
#include "common.h"
#include "ringallocator.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		using WindowSystem::WinWindow;
		using Foundation::RingAllocator;

		extern RingAllocator g_RenderAllocator;

		D3D12Renderer::~D3D12Renderer()
		{
			logger.Log(LogLevel::Info, "Start to clean up render resources.");
			//Note:we should release resources in advance to make REPORT_LIVE_OBJECTS work correctly because if we let the share pointer
			//destructor run out of the scope,we cannot trace the objects 
			//device , swap chain and depth stencil buffer are parent class's members but we still release them here because we need to track alive resources
		}

		D3D12Renderer::D3D12Renderer(const SharedWindowPtr& pWindow, const SharedFileSystemPtr& fs) : Renderer(fs, pWindow)
		{
#ifndef NDEBUG
			EnableDebugLayer();
			HRESULT hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory));
#else
			HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
#endif
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create DXGI factory!");
			}
			logger.Log(LogLevel::Info, "Initialize D3D12 render context succeeded!");
		}

#ifndef NDEBUG
		void D3D12Renderer::EnableDebugLayer()
		{
			auto res = ::D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug));
			if (FAILED(res))
			{
				logger.Log(LogLevel::Warning, "Failed to get d3d12 debug interface!You should enable Graphics Tools optional feature!ErrorCode : 0x%x", res);
			}
			else
			{
				m_d3d12Debug->EnableDebugLayer();
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
			Renderer::ShutDown();
			D3D12RenderTargetManager::Instance()->Clear();
			D3D12DescriptorHeapManager::Instance()->Clear();
			m_dxgiFactory.Reset();
			REPORT_LIVE_OBJECTS;
		}

		ID3D12CommandQueue* D3D12Renderer::GetCommandQueue()
		{
			return static_cast<D3D12Device*>(m_device.get())->GetCommandQueue();
		}

		ID3D12GraphicsCommandList* D3D12Renderer::GetGraphicsCommandList()
		{
			return static_cast<D3D12Device*>(m_device.get())->GetGraphicsCommandList();
		}

		IRenderFence* D3D12Renderer::CreateRenderFence()
		{
			return new D3D12RenderFence(static_cast<D3D12Device*>(m_device.get()), 0);
		}

		IDevice* D3D12Renderer::CreateDevice()
		{
			return new D3D12Device(m_dxgiFactory.Get(), m_fs);
		}

		ISwapChain* D3D12Renderer::CreateSwapChain()
		{
			auto nativeDevice = static_cast<D3D12Device*>(m_device.get())->GetNative();
			return new D3D12SwapChain(m_dxgiFactory.Get(), nativeDevice, GetCommandQueue(), m_outputWindow.get());
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
				logger.Log(LogLevel::Warning, "Can't get dxgidebug.dll module,errorCode:0x%x", ::GetLastError());
				return;
			}
			//the __stdcall declaration is required because windows APIs conform to stdcall convention
			//omit it will cause stack corruption
			using DXGIGetDebugInterfaceFunc = LRESULT (__stdcall *)(REFIID, void**);
			DXGIGetDebugInterfaceFunc pDXGIGetDebugInterface = reinterpret_cast<DXGIGetDebugInterfaceFunc>(::GetProcAddress(dxgiDebugHandle, "DXGIGetDebugInterface"));
			if (!pDXGIGetDebugInterface)
			{
				logger.Log(LogLevel::Warning, "Failed to get debug interface!");
				return;
			}
			pDXGIGetDebugInterface(IID_PPV_ARGS(&m_dxgiDebug));
		}
#endif

		void D3D12Renderer::BeginFrame()
		{
			Renderer::BeginFrame();
		}

		void D3D12Renderer::DoFrame()
		{
			Renderer::DoFrame();
		}

		void D3D12Renderer::EndFrame()
		{
			auto currentSwapChainRT = m_swapChain->GetBufferRenderTarget(m_currentBackBufferIndex);
			auto nativeRT = static_cast<D3D12RenderTarget*>(currentSwapChainRT.get());
			auto commandList = GetGraphicsCommandList();
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRT->GetNative().Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
			commandList->Close();
			ID3D12CommandList* commandListArray[] = { commandList, };
			auto commandQueue = GetCommandQueue();
			commandQueue->ExecuteCommandLists(_countof(commandListArray), commandListArray);
			Renderer::EndFrame();
		}



	}
}