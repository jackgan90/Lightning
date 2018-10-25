#include <cassert>
#include "d3d12renderfence.h"
#include "d3d12renderer.h"

namespace Lightning
{
	namespace Render
	{
		D3D12RenderFence::D3D12RenderFence(D3D12Device* device, std::uint64_t initial_value) : 
			mTargetValue(initial_value)
		{
			mFence = device->CreateFence(initial_value, D3D12_FENCE_FLAG_NONE);
			if (!mFence)
			{
				throw FenceInitDexception("Failed to create d3d12 fence!");
			}
			mEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (!mEvent)
			{
				throw FenceInitDexception("Failed to create fence event!");
			}
		}

		D3D12RenderFence::~D3D12RenderFence()
		{
			mFence.Reset();
			::CloseHandle(mEvent);
		}

		void D3D12RenderFence::SetTargetValue(std::uint64_t value)
		{
			if (value > mFence->GetCompletedValue())
			{
				mTargetValue = value;
				auto commandQueue = static_cast<D3D12Renderer*>(Renderer::Instance())->GetCommandQueue();
				commandQueue->Signal(mFence.Get(), value);
				mFence->SetEventOnCompletion(value, mEvent);
			}
		}

		std::uint64_t D3D12RenderFence::GetTargetValue()
		{
			return mTargetValue;
		}

		std::uint64_t D3D12RenderFence::GetCompletedValue()
		{
			return mFence->GetCompletedValue();
		}


		void D3D12RenderFence::WaitForTarget()
		{
			auto completed_value = mFence->GetCompletedValue();
			while (mTargetValue > completed_value)
			{
				::WaitForSingleObject(mEvent, INFINITE);
				completed_value = mFence->GetCompletedValue();
			}
		}
	}
}