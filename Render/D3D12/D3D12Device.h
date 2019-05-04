#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <atomic>
#include "Device.h"
#include "FileSystem.h"
#include "D3D12Shader.h"
#include "D3D12CommandEncoder.h"
#include "D3D12Texture.h"
#include "D3D12StatefulResource.h"


namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class D3D12Device : public Device
		{
		public:
			friend class D3D12Renderer;
			D3D12Device(IDXGIFactory4* factory);
			~D3D12Device()override;
			std::shared_ptr<IVertexBuffer> CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor)override;
			std::shared_ptr<IIndexBuffer> CreateIndexBuffer(std::uint32_t bufferSize, IndexType type)override;
			std::shared_ptr<IShader> CreateShader(ShaderType type, const std::string& shaderName, 
				const std::string& shaderSource, const std::shared_ptr<IShaderMacros>& macros)override;
			std::shared_ptr<ITexture> CreateTexture(const TextureDescriptor& descriptor, const std::shared_ptr<ISerializeBuffer>& buffer)override;
			std::shared_ptr<IRenderTarget> CreateRenderTarget(const std::shared_ptr<ITexture>& texture) override;
			std::shared_ptr<IDepthStencilBuffer> CreateDepthStencilBuffer(const std::shared_ptr<ITexture>& texture)override;
			std::shared_ptr<D3D12Texture> CreateTexture(const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES initialState);
			//native device method wrapper start
			D3D12StatefulResourcePtr CreateCommittedResource(
				const D3D12_HEAP_PROPERTIES *pHeapProperties,
				D3D12_HEAP_FLAGS HeapFlags,
				const D3D12_RESOURCE_DESC *pDesc,
				D3D12_RESOURCE_STATES InitialResourceState,
				const D3D12_CLEAR_VALUE *pOptimizedClearValue);
			void CreateRenderTargetView( 
				ID3D12Resource *pResource,
				const D3D12_RENDER_TARGET_VIEW_DESC *pDesc,
				D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor);
			void CreateDepthStencilView(
				ID3D12Resource *pResource,
				const D3D12_DEPTH_STENCIL_VIEW_DESC *pDesc,
				D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor);
			void CreateConstantBufferView( 
				const D3D12_CONSTANT_BUFFER_VIEW_DESC *pDesc,
				D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor);
			void CreateShaderResourceView( 
				ID3D12Resource *pResource,
				const D3D12_SHADER_RESOURCE_VIEW_DESC *pDesc,
				D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor);
			UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType);
			ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap( const D3D12_DESCRIPTOR_HEAP_DESC *pDescriptorHeapDesc);
			ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type);
			ComPtr<ID3D12CommandQueue> CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC *pDesc);
			ComPtr<ID3D12CommandList> CreateCommandList( 
				UINT nodeMask,
				D3D12_COMMAND_LIST_TYPE type,
				ID3D12CommandAllocator *pCommandAllocator,
				ID3D12PipelineState *pInitialState);
			HRESULT CheckFeatureSupport( D3D12_FEATURE Feature, void *pFeatureSupportData, UINT FeatureSupportDataSize);
			ComPtr<ID3D12Fence> CreateFence( UINT64 InitialValue, D3D12_FENCE_FLAGS Flags);
			void CreateSampler(const D3D12_SAMPLER_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor);
			ComPtr<ID3D12PipelineState> CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC *pDesc);
			ComPtr<ID3D12RootSignature> CreateRootSignature(
				UINT nodeMask,
				const void *pBlobWithRootSignature,
				SIZE_T blobLengthInBytes);
			void GetCopyableFootprints( 
				const D3D12_RESOURCE_DESC *pResourceDesc,
				UINT FirstSubresource,
				UINT NumSubresources,
				UINT64 BaseOffset,
				D3D12_PLACED_SUBRESOURCE_FOOTPRINT *pLayouts,
				UINT *pNumRows,
				UINT64 *pRowSizeInBytes,
				UINT64 *pTotalBytes);
			//native device method wrapper end
		private:
			//if parameter pState is nullptr,this method will create a default pipeline state
			void CreateNativeDevice(IDXGIFactory4* factory);
			ComPtr<ID3D12Device> mDevice;
			D3D_SHADER_MODEL mHighestShaderModel;
			std::atomic<RenderTargetID> mCurrentRTID;
		};
	}
}
