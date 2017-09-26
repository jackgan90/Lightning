#include "d3d12device.h"
#include "logger.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Utility::LogLevel;
		using Utility::logger;
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
		}

		D3D12Device::~D3D12Device()
		{

		}

		RenderTargetPtr D3D12Device::CreateRenderTarget()
		{
			return RenderTargetPtr();
		}

	}
}