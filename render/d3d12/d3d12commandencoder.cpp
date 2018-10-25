#include "d3d12commandencoder.h"
#include "renderer.h"
#include "d3d12device.h"

namespace Lightning
{
	namespace Render
	{
		D3D12CommandEncoder::D3D12CommandEncoder()
		{
			CreateResources();
		}

		void D3D12CommandEncoder::Close()
		{
			if (mCmdList)
			{
				static_cast<ID3D12GraphicsCommandList*>(mCmdList.Get())->Close();
			}
		}

		void D3D12CommandEncoder::Clear()
		{
			mCmdAllocator.Reset();
			mCmdList.Reset();
		}

		void D3D12CommandEncoder::Reset()
		{
			if (mCmdList)
			{
				mCmdAllocator->Reset();
				static_cast<ID3D12GraphicsCommandList*>(mCmdList.Get())->Reset(mCmdAllocator.Get(), nullptr);
			}
		}

		void D3D12CommandEncoder::CreateResources()
		{
			auto device = Renderer::Instance()->GetDevice();
			if (device)
			{
				auto d3d12Device = static_cast<D3D12Device*>(device);
				if (!mCmdAllocator)
				{
					mCmdAllocator = d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
				}
				if (!mCmdList)
				{
					mCmdList = d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCmdAllocator.Get(), nullptr);
				}
			}
		}


		ID3D12GraphicsCommandList* D3D12CommandEncoder::GetCommandList()
		{
			CreateResources();
			return static_cast<ID3D12GraphicsCommandList*>(mCmdList.Get());
		}
	}
}