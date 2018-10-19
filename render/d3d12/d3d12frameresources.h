#pragma once
#include <d3d12.h>
#include <wrl\client.h>


namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class D3D12FrameResources
		{
		public:
			D3D12FrameResources();
			void Release(bool perFrame);
			ID3D12GraphicsCommandList* GetCommandList();
		private:
			void CreateResources();
			ComPtr<ID3D12CommandAllocator> mCmdAllocator;
			ComPtr<ID3D12GraphicsCommandList> mCmdList;
		};
	}
}
