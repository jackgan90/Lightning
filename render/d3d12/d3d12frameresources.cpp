#include "d3d12frameresources.h"
#include "renderer.h"
#include "d3d12device.h"

namespace Lightning
{
	namespace Render
	{
		D3D12FrameResources::D3D12FrameResources()
		{
			CreateResources();
		}

		void D3D12FrameResources::Reset(bool perFrame)
		{
			mCmdAllocator->Reset();
			if (!perFrame)
			{
				mCmdAllocator.Reset();
				mCmdList.Reset();
			}
			else
			{
				mCmdList->Reset(mCmdAllocator.Get(), nullptr);
			}
		}

		void D3D12FrameResources::CreateResources()
		{
			auto device = Renderer::Instance()->GetDevice();
			auto nativeDevice = static_cast<D3D12Device*>(device)->GetNative();
			if (!mCmdAllocator)
			{
				nativeDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCmdAllocator));
			}
			if (!mCmdList)
			{
				nativeDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&mCmdList));
				mCmdList->Close();
				Reset(true);
			}
		}


		ID3D12GraphicsCommandList* D3D12FrameResources::GetCommandList()
		{
			return mCmdList.Get();
		}
	}
}