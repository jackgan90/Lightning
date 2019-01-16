#pragma once
#include "Container.h"
#include "D3D12Shader.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
		class D3D12ShaderGroup : boost::noncopyable
		{
		public:
			D3D12ShaderGroup();
			~D3D12ShaderGroup();
			//Add a shader to this group,thread unsafe
			void AddShader(D3D12Shader* shader);
			//Commit all shaders managed by this group.Basically just commit all shader parameters.
			void Commit(ID3D12GraphicsCommandList* commandList);
			std::size_t GetHash()const;
			ComPtr<ID3D12RootSignature> CreateRootSignature();
			ComPtr<ID3D12RootSignature> GetRootSignature()const { return mRootSignature; }
		private:
			//Thread unsafe
			void Destroy();
			void CommitDescriptorHeaps(ID3D12GraphicsCommandList* commandList, DescriptorHeap*& constantHeap, DescriptorHeap*& samplerHeap);
			void CommitDescriptorTables(ID3D12GraphicsCommandList* commandList, DescriptorHeap* constantHeap, DescriptorHeap* samplerHeap);
			void CommitConstantBufferDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex,
				const D3D12RootBoundResource& resource, D3D12Device* device,
				CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE& gpuHandle, UINT incrementSize);
			void CommitTextureDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex,
				const D3D12RootBoundResource& resource, D3D12Device* device,
				CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE& gpuHandle, UINT incrementSize);
			void CommitSamplerDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex,
				const D3D12RootBoundResource& resource, D3D12Device* device,
				CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE& gpuHandle, UINT incrementSize);
			
		private:
			Container::Vector<D3D12Shader*> mShaders;
			ComPtr<ID3D12RootSignature> mRootSignature;
			std::size_t mConstantBufferCount;
			std::size_t mTextureCount;
			std::size_t mSamplerStateCount;
		};
	}
}

