#include <d3dx12.h>
#include "d3d12device.h"
#include "d3d12rendertarget.h"
#include "d3d12swapchain.h"
#include "logger.h"
#include "configmanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Foundation::LogLevel;
		using Foundation::logger;
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		D3D12Device::D3D12Device(ComPtr<ID3D12Device> pDevice, D3D12RenderContext* pContext)
		{
			m_context = pContext;
			m_device = pDevice;
			D3D12_COMMAND_QUEUE_DESC desc = {};
			//TODO : here may cause the object he in inconsistent status due to the failure of command queue creation
			HRESULT hr = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
			if (FAILED(hr))
			{
				logger.Log(LogLevel::Error, "Failed to create command queue!");
			}
			const EngineConfig& config = ConfigManager::Instance()->GetConfig();
			for (int i = 0; i < config.SwapChainBufferCount; i++)
			{
				ComPtr<ID3D12CommandAllocator> allocator;
				hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
				if (FAILED(hr))
				{
					logger.Log(LogLevel::Error, "Failed to create command allocator!");
					return;
				}
				m_commandAllocators.push_back(allocator);
			}
			hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_commandList));
			if (FAILED(hr))
			{
				logger.Log(LogLevel::Error, "Failed to create command list!");
				return;
			}
			m_commandList->Close();
		}

		D3D12Device::~D3D12Device()
		{

		}

		void D3D12Device::ReleaseRenderResources()
		{
			m_commandList.Reset();
			//TODO : command allocator should be reset only after the previous frame has finished execution
			for (auto allocator : m_commandAllocators)
				allocator.Reset();
			m_commandAllocators.clear();
			m_commandQueue.Reset();
			m_device.Reset();
		}

		void D3D12Device::ClearRenderTarget(const RenderTargetPtr& rt, const Color& color)
		{
			D3D12RenderTarget *pTarget = static_cast<D3D12RenderTarget*>(rt.get());
			ComPtr<ID3D12Resource> nativeRenderTarget = pTarget->GetNative();
			if (rt->IsSwapChainRenderTarget())
				m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRenderTarget.Get(),
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

			//TODO : check gpu?
			const float clearColor[] = { color(0), color(1), color(2), color(3) };
			auto rtvHandle = pTarget->GetCPUHandle();
			m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
			m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

			if (rt->IsSwapChainRenderTarget())
				m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRenderTarget.Get(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		}
	}
}