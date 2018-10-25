#pragma once
#include <d3d12.h>
#include <wrl\client.h>


namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		//Thread unsafe
		class D3D12CommandEncoder
		{
		public:
			D3D12CommandEncoder();
			//Thread unsafe
			void Reset();
			void Close();
			void Clear();
			ID3D12GraphicsCommandList* GetCommandList();
		private:
			void CreateResources();
			ComPtr<ID3D12CommandAllocator> mCmdAllocator;
			ComPtr<ID3D12CommandList> mCmdList;
		};
	}
}
