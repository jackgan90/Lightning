#include <d3d12.h>
#include <dxgi.h>
#include "common.h"
#include "logger.h"
//#include "rendererfactory.h"
#include "d3d12renderer.h"
#include "d3d12swapchain.h"
#include "d3d12rendertarget.h"
#include "d3d12rendertargetmanager.h"
#include "d3d12descriptorheapmanager.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		D3D12SwapChain::D3D12SwapChain(const ComPtr<IDXGISwapChain3>& pSwapChain, D3D12Renderer* pRenderer):m_renderer(pRenderer)
		{
			m_swapChain = pSwapChain;
			m_swapChain->GetDesc(&m_desc);
			BindRenderTargets();
		}

		D3D12SwapChain::~D3D12SwapChain()
		{
			m_renderTargets.clear();
			m_renderer = nullptr;
		}

		bool D3D12SwapChain::Present()
		{
			HRESULT hr = m_swapChain->Present(0, 0);
			return SUCCEEDED(hr);
		}

		void D3D12SwapChain::BindRenderTargets()
		{
			HRESULT hr;
			auto d3ddevice = static_cast<D3D12Device*>(m_renderer->GetDevice());
			auto nativeDevice = d3ddevice->GetNative();
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			desc.NumDescriptors = RENDER_FRAME_COUNT;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			const HeapAllocationInfo* pHeapInfo = D3D12DescriptorHeapManager::Instance()->Create(desc);
			if (!pHeapInfo)
			{
				throw SwapChainInitException("Failed to allocate RTV descriptor heap!");
			}
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pHeapInfo->cpuHeapStart);

			ComPtr<ID3D12Resource> swapChainTargets[RENDER_FRAME_COUNT];
			auto rtMgr = D3D12RenderTargetManager::Instance();
			for (int i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainTargets[i]));
				if (FAILED(hr))
				{
					throw SwapChainInitException("Failed to get d3d12 swap chain buffer.");
				}
				nativeDevice->CreateRenderTargetView(swapChainTargets[i].Get(), nullptr, rtvHandle);
				auto rt = rtMgr->CreateSwapChainRenderTarget(swapChainTargets[i], rtvHandle, this);
				m_renderTargets[i] = rt->GetID();
				rtvHandle.Offset(pHeapInfo->incrementSize);
			}
		}

		SharedRenderTargetPtr D3D12SwapChain::GetBufferRenderTarget(unsigned int bufferIndex)
		{
			auto it = m_renderTargets.find(bufferIndex);
			if (it == m_renderTargets.end())
				return SharedRenderTargetPtr();
			return D3D12RenderTargetManager::Instance()->GetRenderTarget(it->second);
		}
	}
}