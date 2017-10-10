#include <d3d12.h>
#include <dxgi.h>
#include "common.h"
#include "logger.h"
//#include "rendererfactory.h"
#include "irenderer.h"
#include "d3d12swapchain.h"
#include "d3d12rendertarget.h"
#include "d3d12rendertargetmanager.h"
#include "d3d12descriptorheapmanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		D3D12SwapChain::D3D12SwapChain(ComPtr<IDXGISwapChain3> pSwapChain, D3D12Renderer* pRenderer):m_renderer(pRenderer)
		{
			m_swapChain = pSwapChain;
			DXGI_SWAP_CHAIN_DESC desc;
			pSwapChain->GetDesc(&desc);
			m_bufferCount = desc.BufferCount;
			BindRenderTargets();
		}

		D3D12SwapChain::~D3D12SwapChain()
		{
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
			auto d3ddevice = STATIC_CAST_PTR(D3D12Device, m_renderer->GetDevice());
			ComPtr<ID3D12Device> pd3ddevice = d3ddevice->m_device;
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			int renderTargetCount = GetBufferCount();
			desc.NumDescriptors = renderTargetCount;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			const HeapAllocationInfo* pHeapInfo = m_renderer->GetDescriptorHeapManager()->Create(desc);
			if (!pHeapInfo)
			{
				throw SwapChainInitException("Failed to allocate RTV descriptor heap!");
			}
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pHeapInfo->cpuHeapStart);

			ComPtr<ID3D12Resource>* swapChainTargets = new ComPtr<ID3D12Resource>[renderTargetCount];
			m_rtMgr = std::make_shared<D3D12RenderTargetManager>(d3ddevice);
			for (int i = 0; i < renderTargetCount; i++)
			{
				hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainTargets[i]));
				if (FAILED(hr))
				{
					delete []swapChainTargets;
					throw SwapChainInitException("Failed to get d3d12 swap chain buffer.");
				}
				pd3ddevice->CreateRenderTargetView(swapChainTargets[i].Get(), nullptr, rtvHandle);
				auto rt = m_rtMgr->CreateSwapChainRenderTarget(swapChainTargets[i], rtvHandle);
				m_renderTargets[i] = rt->GetID();
				rtvHandle.Offset(pHeapInfo->incrementSize);
			}
			delete []swapChainTargets;
		}

		RenderTargetPtr D3D12SwapChain::GetBufferRenderTarget(unsigned int bufferIndex)
		{
			auto it = m_renderTargets.find(bufferIndex);
			if (it == m_renderTargets.end())
				return RenderTargetPtr();
			return m_rtMgr->GetRenderTarget(it->second);
		}
	}
}