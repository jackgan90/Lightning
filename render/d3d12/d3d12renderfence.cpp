#include <cassert>
#include "d3d12renderfence.h"

namespace Lightning
{
	namespace Render
	{
		D3D12RenderFence::D3D12RenderFence(D3D12Device* device, std::uint64_t initial_value) : 
			m_device(device), m_targetValue(initial_value)
		{
			auto nativeDevice = device->GetNative();
			auto hr = nativeDevice->CreateFence(initial_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
			if (FAILED(hr))
			{
				throw FenceInitDexception("Failed to create d3d12 fence!");
			}
			m_event = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (!m_event)
			{
				throw FenceInitDexception("Failed to create fence event!");
			}
		}

		D3D12RenderFence::~D3D12RenderFence()
		{
			m_fence.Reset();
			::CloseHandle(m_event);
		}

		void D3D12RenderFence::SetTargetValue(std::uint64_t value)
		{
			if (value > m_fence->GetCompletedValue())
			{
				m_targetValue = value;
				auto commandQueue = m_device->GetCommandQueue();
				commandQueue->Signal(m_fence.Get(), value);
				m_fence->SetEventOnCompletion(value, m_event);
			}
		}

		std::uint64_t D3D12RenderFence::GetTargetValue()
		{
			return m_targetValue;
		}

		std::uint64_t D3D12RenderFence::GetCurrentValue()
		{
			return m_fence->GetCompletedValue();
		}


		void D3D12RenderFence::WaitForTarget()
		{
			auto completed_value = m_fence->GetCompletedValue();
			while (m_targetValue > completed_value)
			{
				::WaitForSingleObject(m_event, INFINITE);
				completed_value = m_fence->GetCompletedValue();
			}
		}
	}
}