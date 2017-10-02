#include "d3d12device.h"
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
		D3D12Device::D3D12Device(ComPtr<ID3D12Device> pDevice)
		{
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
				HRESULT hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
				if (FAILED(hr))
				{
					logger.Log(LogLevel::Error, "Failed to create command allocator!");
					break;
				}
				m_commandAllocators.push_back(allocator);
			}
		}

		D3D12Device::~D3D12Device()
		{

		}

		void D3D12Device::ReleaseRenderResources()
		{
			m_commandQueue.Reset();
			for (auto allocator : m_commandAllocators)
				allocator.Reset();
			m_commandAllocators.clear();
		}
	}
}