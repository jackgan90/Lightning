#include <d3dx12.h>
#include <d3dcompiler.h>
#include <functional>
#include "Common.h"
#include "RenderConstants.h"
#include "D3D12Renderer.h"
#include "D3D12Device.h"
#include "D3D12RenderTarget.h"
#include "D3D12DepthStencilBuffer.h"
#include "D3D12TypeMapper.h"
#include "D3D12VertexBuffer.h"
#include "D3D12IndexBuffer.h"
#include "D3D12Texture.h"
#include "RenderConstants.h"
#include "Semantics.h"
#include "Logger.h"
#include "ConfigManager.h"
#include "FrameMemoryAllocator.h"
#include "D3D12ConstantBufferManager.h"
#include "tbb/spin_mutex.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;

		extern FrameMemoryAllocator g_RenderAllocator;
		const char* const DEFAULT_VS_SOURCE =
			"cbuffer VSConstants : register(b0)\n"
			"{\n"
			"	float4x4 wvp;\n"
			"};\n"
			"struct VSInput\n"
			"{\n"
			"	float3 position : POSITION;\n"
			"	float3 normal : NORMAL;\n"
			"};\n"
			"struct VSOutput\n"
			"{\n"
			"	float4 pos : SV_POSITION;\n"
			"	float3 normal : TEXCOORD0;\n"
			"};\n"
			"VSOutput main(VSInput input)\n"
			"{\n"
			"	VSOutput output;\n"
			"	output.pos = mul(float4(input.position, 1.0f), wvp);\n"
			"	output.normal = input.normal;\n"
			"	return output;\n"
			"}\n";
		const char* const DEFAULT_PS_SOURCE =
			"cbuffer PSConstants : register(b0)\n"
			"{\n"
			"	float4 color;\n"
			"	float3 light;\n"
			"};\n"
			"//cbuffer PSConstants1 : register(b1)\n"
			"//{\n"
			"//	float3 light;\n"
			"//};\n"
			"struct PSInput\n"
			"{\n"
			"	float4 pos : SV_POSITION;\n"
			"	float3 normal : TEXCOORD0;\n"
			"};\n"
			"float4 main(PSInput input):SV_TARGET\n"
			"{\n"
			"	float3 N = normalize(input.normal);\n"
			"	float3 L = normalize(light);\n"
			"	float3 diffuse = dot(N, L);\n"
			"	return float4(color.rgb * diffuse, color.a) ;\n"
			"}\n";
		D3D12Device::D3D12Device(IDXGIFactory4* factory)
			:Device(), mCurrentRTID(1)
		{
			CreateNativeDevice(factory);
			D3D12_FEATURE_DATA_SHADER_MODEL featureShaderModel;
			featureShaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_2;
			auto res = CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &featureShaderModel, sizeof(featureShaderModel));
			mHighestShaderModel = featureShaderModel.HighestShaderModel;
			D3D12_COMMAND_QUEUE_DESC desc = {};
			//should create first default pipeline state
			mDefaultShaders[ShaderType::VERTEX] = CreateShader(ShaderType::VERTEX, "[Built-in]default.vs", DEFAULT_VS_SOURCE, nullptr);
			mDefaultShaders[ShaderType::FRAGMENT] = CreateShader(ShaderType::FRAGMENT, "[Built-in]default.ps", DEFAULT_PS_SOURCE, nullptr);
		}

		D3D12Device::~D3D12Device()
		{
		}

		void D3D12Device::CreateNativeDevice(IDXGIFactory4* factory)
		{
			ComPtr<IDXGIAdapter1> adaptor;
			int adaptorIndex = 0;
			bool adaptorFound = false;
			HRESULT hr;

			while (factory->EnumAdapters1(adaptorIndex, &adaptor) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC1 desc;
				adaptor->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					adaptorIndex++;
					continue;
				}
				hr = D3D12CreateDevice(adaptor.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
				if (SUCCEEDED(hr))
				{
					adaptorFound = true;
					break;
				}
				adaptorIndex++;
			}
			if (!adaptorFound)
			{
				LOG_ERROR("Can't find hardware d3d12 adaptor!");
				return;
			}
			hr = D3D12CreateDevice(adaptor.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice));
			if (FAILED(hr))
			{
				LOG_ERROR("Failed to create d3d12 device!");
			}
		}




		std::shared_ptr<IVertexBuffer> D3D12Device::CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor)
		{
			return std::make_shared<D3D12VertexBuffer>(this, bufferSize, descriptor);
		}

		std::shared_ptr<IIndexBuffer> D3D12Device::CreateIndexBuffer(std::uint32_t bufferSize, IndexType type)
		{
			return std::make_shared<D3D12IndexBuffer>(this, bufferSize, type);
		}


		std::shared_ptr<IShader> D3D12Device::CreateShader(ShaderType type, const std::string& shaderName, 
			const std::string& shaderSource, const std::shared_ptr<IShaderMacros>& macros)
		{
			return std::make_shared<D3D12Shader>(mHighestShaderModel, type, shaderName, shaderSource, macros);
		}

		std::shared_ptr<ITexture> D3D12Device::CreateTexture(const TextureDescriptor& descriptor, const std::shared_ptr<ISerializeBuffer>& buffer)
		{
			D3D12_RESOURCE_DESC desc{};
			desc.Alignment = 0;
			if (descriptor.dimension == TEXTURE_DIMENSION_1D_ARRAY || descriptor.dimension == TEXTURE_DIMENSION_2D_ARRAY)
			{
				desc.DepthOrArraySize = UINT16(descriptor.arraySize);
			}
			else
			{
				desc.DepthOrArraySize = UINT16(descriptor.depth);
			}
			desc.Dimension = D3D12TypeMapper::MapTextureDimension(descriptor.dimension);
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;
			desc.Format = D3D12TypeMapper::MapRenderFormat(descriptor.format);
			desc.Height = UINT(descriptor.height);
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			desc.MipLevels = UINT16(descriptor.numberOfMipmaps);
			desc.SampleDesc.Count = UINT(descriptor.multiSampleCount);
			desc.SampleDesc.Quality = UINT(descriptor.multiSampleQuality);
			desc.Width = descriptor.width;
			if (desc.Format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT ||
				desc.Format == DXGI_FORMAT_D32_FLOAT ||
				desc.Format == DXGI_FORMAT_D24_UNORM_S8_UINT ||
				desc.Format == DXGI_FORMAT_D16_UNORM)
			{
				//depth stencil texture
				desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
				return std::make_shared<D3D12Texture>(desc, this, descriptor.depthClearValue, descriptor.stencilClearValue);
			}
			else
			{
				//Non depth stencil texture
				return std::make_shared<D3D12Texture>(desc, this, buffer);
			}
		}

		std::shared_ptr<D3D12Texture> D3D12Device::CreateTexture(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState)
		{
			return std::make_shared<D3D12Texture>(this, resource, initialState);
		}

		std::shared_ptr<IRenderTarget> D3D12Device::CreateRenderTarget(const std::shared_ptr<ITexture>& texture)
		{
			auto D3DTexture = std::dynamic_pointer_cast<D3D12Texture>(texture);//dynamic_cast<D3D12Texture*>(texture);
			assert(D3DTexture && "A D3D12Texture is required!");
			auto id = mCurrentRTID.fetch_add(1, std::memory_order_relaxed);
			return std::make_shared<D3D12RenderTarget>(id, D3DTexture);
		}

		std::shared_ptr<IDepthStencilBuffer> D3D12Device::CreateDepthStencilBuffer(const std::shared_ptr<ITexture>& texture)
		{
			auto D3DTexture = std::dynamic_pointer_cast<D3D12Texture>(texture);//dynamic_cast<D3D12Texture*>(texture);
			assert(D3DTexture && "A D3D12Texture is required.");
			//return NEW_REF_OBJ(D3D12DepthStencilBuffer, D3DTexture);
			return std::make_shared<D3D12DepthStencilBuffer>(D3DTexture);
		}

		//native device method wrappers begin
		D3D12StatefulResourcePtr D3D12Device::CreateCommittedResource(
			const D3D12_HEAP_PROPERTIES *pHeapProperties,
			D3D12_HEAP_FLAGS HeapFlags,
			const D3D12_RESOURCE_DESC *pDesc,
			D3D12_RESOURCE_STATES InitialResourceState,
			const D3D12_CLEAR_VALUE *pOptimizedClearValue)
		{
			ComPtr<ID3D12Resource> resource;
			mDevice->CreateCommittedResource(pHeapProperties, HeapFlags, pDesc,
				InitialResourceState, pOptimizedClearValue, IID_PPV_ARGS(&resource));

			return std::make_shared<D3D12StatefulResource>(resource, InitialResourceState);
		}

		void D3D12Device::CreateRenderTargetView(
			ID3D12Resource *pResource,
			const D3D12_RENDER_TARGET_VIEW_DESC *pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
		{
			mDevice->CreateRenderTargetView(pResource, pDesc, DestDescriptor);
		}

		void D3D12Device::CreateDepthStencilView(
			ID3D12Resource *pResource,
			const D3D12_DEPTH_STENCIL_VIEW_DESC *pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
		{
			mDevice->CreateDepthStencilView(pResource, pDesc, DestDescriptor);
		}

		void D3D12Device::CreateConstantBufferView(
			const D3D12_CONSTANT_BUFFER_VIEW_DESC *pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
		{
			mDevice->CreateConstantBufferView(pDesc, DestDescriptor);
		}

		void D3D12Device::CreateShaderResourceView(
			ID3D12Resource *pResource,
			const D3D12_SHADER_RESOURCE_VIEW_DESC *pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
		{
			mDevice->CreateShaderResourceView(pResource, pDesc, DestDescriptor);
		}

		UINT D3D12Device::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType)
		{
			return mDevice->GetDescriptorHandleIncrementSize(DescriptorHeapType);
		}

		ComPtr<ID3D12DescriptorHeap> D3D12Device::CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC *pDescriptorHeapDesc)
		{
			ComPtr<ID3D12DescriptorHeap> heap;
			mDevice->CreateDescriptorHeap(pDescriptorHeapDesc, IID_PPV_ARGS(&heap));
			return heap;
		}

		ComPtr<ID3D12CommandAllocator> D3D12Device::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type)
		{
			ComPtr<ID3D12CommandAllocator> commandAllocator;
			mDevice->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));
			return commandAllocator;
		}

		ComPtr<ID3D12CommandQueue> D3D12Device::CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC *pDesc)
		{
			ComPtr<ID3D12CommandQueue> queue;
			mDevice->CreateCommandQueue(pDesc, IID_PPV_ARGS(&queue));
			return queue;
		}

		ComPtr<ID3D12CommandList> D3D12Device::CreateCommandList(
			UINT nodeMask,
			D3D12_COMMAND_LIST_TYPE type,
			ID3D12CommandAllocator *pCommandAllocator,
			ID3D12PipelineState *pInitialState)
		{
			ComPtr<ID3D12CommandList> commandList;
			mDevice->CreateCommandList(nodeMask, type, pCommandAllocator, pInitialState, IID_PPV_ARGS(&commandList));
			return commandList;
		}

		HRESULT D3D12Device::CheckFeatureSupport(D3D12_FEATURE Feature, void *pFeatureSupportData, UINT FeatureSupportDataSize)
		{
			return mDevice->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
		}

		ComPtr<ID3D12Fence> D3D12Device::CreateFence(UINT64 InitialValue, D3D12_FENCE_FLAGS Flags)
		{
			ComPtr<ID3D12Fence> fence;
			mDevice->CreateFence(InitialValue, Flags, IID_PPV_ARGS(&fence));
			return fence;
		}

		void D3D12Device::CreateSampler(const D3D12_SAMPLER_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
		{
			mDevice->CreateSampler(pDesc, DestDescriptor);
		}

		ComPtr<ID3D12PipelineState> D3D12Device::CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC *pDesc)
		{
			ComPtr<ID3D12PipelineState> pipelineState;
			mDevice->CreateGraphicsPipelineState(pDesc, IID_PPV_ARGS(&pipelineState));

			return pipelineState;
		}

		ComPtr<ID3D12RootSignature> D3D12Device::CreateRootSignature(
			UINT nodeMask,
			const void *pBlobWithRootSignature,
			SIZE_T blobLengthInBytes)
		{
			ComPtr<ID3D12RootSignature> rootSignature;
			mDevice->CreateRootSignature(nodeMask, pBlobWithRootSignature, blobLengthInBytes, IID_PPV_ARGS(&rootSignature));

			return rootSignature;
		}

		void D3D12Device::GetCopyableFootprints(
			const D3D12_RESOURCE_DESC *pResourceDesc,
			UINT FirstSubresource,
			UINT NumSubresources,
			UINT64 BaseOffset,
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT *pLayouts,
			UINT *pNumRows,
			UINT64 *pRowSizeInBytes,
			UINT64 *pTotalBytes)
		{
			mDevice->GetCopyableFootprints(pResourceDesc, FirstSubresource, NumSubresources,
				BaseOffset, pLayouts, pNumRows, pRowSizeInBytes, pTotalBytes);
		}

		//native device method wrappers end
	}
}