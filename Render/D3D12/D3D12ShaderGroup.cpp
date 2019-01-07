#include "D3D12Renderer.h"
#include "D3D12ShaderGroup.h"

namespace Lightning
{
	namespace Render
	{
		D3D12ShaderGroup::D3D12ShaderGroup()
			: mConstantBufferCount(0)
			, mTextureCount(0)
			, mSamplerStateCount(0)
		{

		}

		D3D12ShaderGroup::~D3D12ShaderGroup()
		{
			Destroy();
		}

		void D3D12ShaderGroup::AddShader(D3D12Shader* shader)
		{
			if (!shader)
				return;
			shader->AddRef();
			mConstantBufferCount += shader->GetConstantBufferCount();
			mTextureCount += shader->GetTextureCount();
			mSamplerStateCount += shader->GetSamplerStateCount();
			mShaders.push_back(shader);
		}

		void D3D12ShaderGroup::Destroy()
		{
			for (auto& descriptorArray : mDescriptorHeaps)
			{
				descriptorArray.for_each([](DescriptorHeapArray& descriptorHeaps) {
					for (auto i = 0;i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
					{
						if (descriptorHeaps.heaps[i])
						{
							D3D12DescriptorHeapManager::Instance()->Deallocate(descriptorHeaps.heaps[i]);
							descriptorHeaps.heaps[i] = nullptr;
						}
					}
					descriptorHeaps.heapList.clear();
				});
			}

			for (auto shader : mShaders)
			{
				shader->Release();
			}
			mConstantBufferCount = 0;
			mTextureCount = 0;
			mSamplerStateCount = 0;
			mShaders.clear();
		}

		void D3D12ShaderGroup::Commit()
		{
			auto renderer = static_cast<D3D12Renderer*>(Renderer::Instance());
			auto device = static_cast<D3D12Device*>(renderer->GetDevice());
			auto commandList = renderer->GetGraphicsCommandList();
			auto frameResourceIndex = renderer->GetFrameResourceIndex();
			auto& descriptorHeaps = *mDescriptorHeaps[frameResourceIndex];
			//the heap to store CBVs SRVs UAVs
			DescriptorHeap* constantHeap{ nullptr };
			if (mConstantBufferCount + mTextureCount > 0)
			{
				constantHeap = descriptorHeaps.heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
				if (!constantHeap)
				{
					constantHeap = D3D12DescriptorHeapManager::Instance()->Allocate(
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true, UINT(mConstantBufferCount + mTextureCount), false);
					descriptorHeaps.heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = constantHeap;
				}
			}
			descriptorHeaps.heapList.clear();
			for (auto heap : descriptorHeaps.heaps)
			{
				if (heap)
				{
					descriptorHeaps.heapList.push_back(
						D3D12DescriptorHeapManager::Instance()->GetHeap(heap).Get()
					);
				}
			}
			if (!descriptorHeaps.heapList.empty())
			{
				commandList->SetDescriptorHeaps(UINT(descriptorHeaps.heapList.size()), &descriptorHeaps.heapList[0]);
			}
			UINT rootParameterIndex{ 0 };
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;
			if (constantHeap)
			{
				cpuHandle = constantHeap->cpuHandle;
				gpuHandle = constantHeap->gpuHandle;
			}
			for (auto shader : mShaders)
			{
				auto boundResources = shader->GetRootBoundResources();
				for (auto i = 0;i < shader->GetRootParameterCount();++i)
				{
					const auto& resource = boundResources[i];
					if (boundResources[i].type == D3D12RootResourceType::ConstantBuffers)
					{
						commandList->SetGraphicsRootDescriptorTable(rootParameterIndex++, gpuHandle);
						for (auto i = 0;i < resource.count;++i)
						{
							const auto& cbuffer = resource.buffers[i];
							D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
							cbvDesc.BufferLocation = cbuffer.virtualAdress;
							cbvDesc.SizeInBytes = UINT(cbuffer.size);
							device->CreateConstantBufferView(&cbvDesc, cpuHandle);
							cpuHandle.Offset(constantHeap->incrementSize);
							gpuHandle.Offset(constantHeap->incrementSize);
						}
					}
				}
			}
		}
	}
}
