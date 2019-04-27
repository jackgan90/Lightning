#include "Logger.h"
#include "D3D12Renderer.h"
#include "D3D12ShaderGroup.h"
#include "D3D12TypeMapper.h"
#include "D3D12DescriptorHeapManager.h"

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
			for (auto shader : mShaders)
			{
				shader->Release();
			}
			mConstantBufferCount = 0;
			mTextureCount = 0;
			mSamplerStateCount = 0;
			mShaders.clear();
		}

		void D3D12ShaderGroup::CommitDescriptorHeaps(ID3D12GraphicsCommandList* commandList, 
			DescriptorHeap*& constantHeap, DescriptorHeap*& samplerHeap)
		{
			using DescriptorHeapLists = Foundation::ThreadLocalObject<std::vector<ID3D12DescriptorHeap*>>;
			static DescriptorHeapLists descriptorHeapLists;
			auto& descriptorHeaps = *descriptorHeapLists;
			descriptorHeaps.clear();
			//the heap to store CBVs SRVs UAVs
			constantHeap = nullptr;
			samplerHeap = nullptr;
			if (mConstantBufferCount + mTextureCount > 0)
			{
				constantHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
					true, UINT(mConstantBufferCount + mTextureCount), true);
				descriptorHeaps.push_back(D3D12DescriptorHeapManager::Instance()->GetHeap(constantHeap).Get());
			}
			if (mSamplerStateCount > 0)
			{
				samplerHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
					true, UINT(mSamplerStateCount), true);
				descriptorHeaps.push_back(D3D12DescriptorHeapManager::Instance()->GetHeap(samplerHeap).Get());
			}
			if (!descriptorHeaps.empty())
			{
				commandList->SetDescriptorHeaps(UINT(descriptorHeaps.size()), &descriptorHeaps[0]);
			}
		}

		void D3D12ShaderGroup::CommitDescriptorTables(ID3D12GraphicsCommandList* commandList
			, DescriptorHeap* constantHeap, DescriptorHeap* samplerHeap)
		{
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			UINT rootParameterIndex{ 0 };
			CD3DX12_CPU_DESCRIPTOR_HANDLE constantCPUHandle, samplerCPUHandle;
			CD3DX12_GPU_DESCRIPTOR_HANDLE constantGPUHandle, samplerGPUHandle;
			if (constantHeap)
			{
				constantCPUHandle = constantHeap->CPUHandle;
				constantGPUHandle = constantHeap->GPUHandle;
			}
			if (samplerHeap)
			{
				samplerCPUHandle = samplerHeap->CPUHandle;
				samplerGPUHandle = samplerHeap->GPUHandle;
			}
			for (auto shader : mShaders)
			{
				auto boundResources = shader->GetRootBoundResources();
				for (auto i = 0;i < shader->GetRootParameterCount();++i)
				{
					const auto& resource = boundResources[i];
					if (resource.type == D3D12RootResourceType::ConstantBuffers)
					{
						CommitConstantBufferDescriptorTable(commandList, rootParameterIndex++,
							resource, device, constantCPUHandle, constantGPUHandle, constantHeap->incrementSize);
					}
					else if (resource.type == D3D12RootResourceType::Textures)
					{
						CommitTextureDescriptorTable(commandList, rootParameterIndex++,
							resource, device, constantCPUHandle, constantGPUHandle, constantHeap->incrementSize);
					}
					else if (resource.type == D3D12RootResourceType::Samplers)
					{
						CommitSamplerDescriptorTable(commandList, rootParameterIndex++,
							resource, device, samplerCPUHandle, samplerGPUHandle, samplerHeap->incrementSize);
					}
				}
			}
		}

		void D3D12ShaderGroup::CommitConstantBufferDescriptorTable(
			ID3D12GraphicsCommandList* commandList
			, UINT rootParameterIndex
			, const D3D12RootBoundResource& resource
			, D3D12Device* device
			, CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuHandle
			, CD3DX12_GPU_DESCRIPTOR_HANDLE& gpuHandle
			, UINT incrementSize)
		{
			commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle);
			for (auto i = 0;i < resource.count;++i)
			{
				const auto& cbuffer = resource.buffers[i];
				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
				cbvDesc.BufferLocation = cbuffer.virtualAdress;
				cbvDesc.SizeInBytes = UINT(cbuffer.size);
				device->CreateConstantBufferView(&cbvDesc, cpuHandle);
				cpuHandle.Offset(incrementSize);
				gpuHandle.Offset(incrementSize);
			}
		}

		void D3D12ShaderGroup::CommitTextureDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex,
			const D3D12RootBoundResource& resource, D3D12Device* device,
			CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE& gpuHandle, UINT incrementSize)
		{
			commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle);
			for (auto i = 0;i < resource.count;++i)
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
				auto textureFormat = resource.textures[i]->GetRenderFormat();
				srvDesc.Format = D3D12TypeMapper::MapRenderFormat(textureFormat);
				bool isMultiSampled = resource.textures[i]->GetMultiSampleCount() > 1;
				auto textureDimension = resource.textures[i]->GetDimension();
				srvDesc.ViewDimension = D3D12TypeMapper::MapSRVDimension(textureDimension, isMultiSampled);
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				SetSRVTextureParams(srvDesc, resource.textures[i]);
				auto D3DResource = resource.textures[i]->GetResource()->GetResource();
				device->CreateShaderResourceView(D3DResource, &srvDesc, cpuHandle);
				cpuHandle.Offset(incrementSize);
				gpuHandle.Offset(incrementSize);
			}
		}

		void D3D12ShaderGroup::CommitSamplerDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex,
			const D3D12RootBoundResource& resource, D3D12Device* device,
			CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE& gpuHandle, UINT incrementSize)
		{
			commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle);
			for (auto i = 0;i < resource.count;++i)
			{
				D3D12_SAMPLER_DESC D3D12SamplerState;
				const auto& samplerState = resource.samplerStates[i];
				D3D12SamplerState.AddressU = D3D12TypeMapper::MapAddressMode(samplerState.addressU);
				D3D12SamplerState.AddressV = D3D12TypeMapper::MapAddressMode(samplerState.addressV);
				D3D12SamplerState.AddressW = D3D12TypeMapper::MapAddressMode(samplerState.addressW);
				std::memcpy(D3D12SamplerState.BorderColor, samplerState.borderColor, sizeof(samplerState.borderColor));
				//TODO : figure out the real meaning of ComparisionFunc for D3D12_SAMPLER_DESC
				D3D12SamplerState.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
				D3D12SamplerState.Filter = D3D12TypeMapper::MapSamplerFilterMode(samplerState.filterMode);
				D3D12SamplerState.MaxAnisotropy = samplerState.maxAnisotropy;
				D3D12SamplerState.MaxLOD = samplerState.maxLOD;
				D3D12SamplerState.MinLOD = samplerState.minLOD;
				D3D12SamplerState.MipLODBias = samplerState.mipLODBias;
				device->CreateSampler(&D3D12SamplerState, cpuHandle);
				cpuHandle.Offset(incrementSize);
				gpuHandle.Offset(incrementSize);
			}
		}

		void D3D12ShaderGroup::Commit(ID3D12GraphicsCommandList* commandList)
		{
			auto renderer = static_cast<D3D12Renderer*>(Renderer::Instance());
			DescriptorHeap* constantHeap{ nullptr }, *samplerHeap{ nullptr };
			CommitDescriptorHeaps(commandList, constantHeap, samplerHeap);
			CommitDescriptorTables(commandList, constantHeap, samplerHeap);
		}

		std::size_t D3D12ShaderGroup::GetHash()const
		{
			size_t seed = 0;
			boost::hash_combine(seed, mShaders.size());
			for (const auto& s : mShaders)
			{
				boost::hash_combine(seed, s->GetHash());
			}
			return seed;
		}

		ComPtr<ID3D12RootSignature> D3D12ShaderGroup::CreateRootSignature()
		{
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			std::vector<D3D12_ROOT_PARAMETER> rootParameters;
			D3D12_ROOT_SIGNATURE_FLAGS flags = 
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | 
				D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS | 
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
			for (std::size_t i = 0;i < mShaders.size();++i)
			{
				switch (mShaders[i]->GetType())
				{
				case ShaderType::VERTEX:
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::FRAGMENT:
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::GEOMETRY:
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::HULL:		//hull
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::DOMAIN:	//domain
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
					break;
				}
				const auto& parameters = static_cast<D3D12Shader*>(mShaders[i])->GetRootParameters();
				rootParameters.insert(rootParameters.end(), parameters.begin(), parameters.end());
			}
			D3D12_ROOT_PARAMETER* pParameters = rootParameters.empty() ? nullptr : &rootParameters[0];
			rootSignatureDesc.Init(UINT(rootParameters.size()), pParameters, 0, nullptr, flags);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			auto hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
			if (FAILED(hr))
			{
				if (error)
				{
					auto buffer = static_cast<char*>(error->GetBufferPointer());
					LOG_ERROR("Failed to serialize root signature,reason : {0}.", buffer);
				}
				return ComPtr<ID3D12RootSignature>();
			}

			mRootSignature = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize());
			return mRootSignature;
		}

		void D3D12ShaderGroup::SetSRVTextureParams(D3D12_SHADER_RESOURCE_VIEW_DESC& desc, D3D12Texture* texture)
		{
			auto dimension = texture->GetDimension();
			if (dimension == TEXTURE_DIMENSION_2D)
			{
				desc.Texture2D.MipLevels = UINT(texture->GetMipmapLevels());
				desc.Texture2D.MostDetailedMip = 0;
			}
		}
	}
}
