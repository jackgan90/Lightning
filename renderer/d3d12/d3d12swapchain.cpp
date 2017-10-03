#include <d3d12.h>
#include <dxgi.h>
#include "common.h"
#include "logger.h"
#include "rendererfactory.h"
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
		D3D12SwapChain::D3D12SwapChain(ComPtr<IDXGISwapChain3> pSwapChain, D3D12Device* pDevice):m_device(pDevice)
		{
			m_swapChain = pSwapChain;
			DXGI_SWAP_CHAIN_DESC desc;
			pSwapChain->GetDesc(&desc);
			m_bufferCount = desc.BufferCount;
		}

		D3D12SwapChain::~D3D12SwapChain()
		{

		}

		bool D3D12SwapChain::Present()
		{
			HRESULT hr = m_swapChain->Present(0, 0);
			return SUCCEEDED(hr);
		}

		void D3D12SwapChain::ReleaseRenderResources()
		{
			m_swapChain.Reset();
		}

		bool D3D12SwapChain::BindSwapChainRenderTargets()
		{
			HRESULT hr;
			ComPtr<ID3D12Device> pd3ddevice = m_device->m_device;
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			int renderTargetCount = GetBufferCount();
			desc.NumDescriptors = renderTargetCount;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;


			const HeapAllocationInfo* pHeapInfo = D3D12DescriptorHeapManager::Instance()->Create(desc);
			if (!pHeapInfo)
				return false;
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pHeapInfo->cpuHeapStart);

			ComPtr<ID3D12Resource>* swapChainTargets = new ComPtr<ID3D12Resource>[renderTargetCount];
			D3D12RenderTargetManager* pRTManager = DYNAMIC_CAST_PTR(D3D12RenderTargetManager, RendererFactory::GetRenderTargetManager());
			for (int i = 0; i < renderTargetCount; i++)
			{
				hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainTargets[i]));
				if (FAILED(hr))
				{
					logger.Log(LogLevel::Error, "Failed to get d3d12 swapchain buffer %d", i);
					delete []swapChainTargets;
					return false;
				}
				pd3ddevice->CreateRenderTargetView(swapChainTargets[i].Get(), nullptr, rtvHandle);
				auto rt = pRTManager->CreateSwapChainRenderTarget(swapChainTargets[i], rtvHandle);
				m_renderTargets[i] = rt->GetID();
				rtvHandle.Offset(pHeapInfo->incrementSize);
			}
			delete []swapChainTargets;
			return true;
		}

		RenderTargetPtr D3D12SwapChain::GetBufferRenderTarget(unsigned int bufferIndex)
		{
			auto it = m_renderTargets.find(bufferIndex);
			if (it == m_renderTargets.end())
				return RenderTargetPtr();
			return RendererFactory::GetRenderTargetManager()->GetRenderTarget(it->second);
		}
	}
}