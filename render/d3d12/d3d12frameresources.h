#pragma once
#include <d3d12.h>
#include <wrl\client.h>


namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		//Thread unsafe
		class D3D12FrameResources
		{
		public:
			D3D12FrameResources();
			//Thread unsafe
			void Reset(bool perFrame);
			void Close();
			ID3D12GraphicsCommandList* GetCommandList();
		private:
			void CreateResources();
			ComPtr<ID3D12CommandAllocator> mCmdAllocator;
			ComPtr<ID3D12CommandList> mCmdList;
		};
	}
}
