#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <memory>
#include "threadlocalsingleton.h"
#include "container.h"
#include "device.h"
#include "stackallocator.h"
#include "filesystem.h"
#include "d3d12shader.h"
#include "d3d12shadermanager.h"
#include "d3d12frameresources.h"


namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::SharedFileSystemPtr;
		using Foundation::StackAllocator;
		using Foundation::container;
		class LIGHTNING_RENDER_API D3D12Device : public Device
		{
		public:
			friend class D3D12Renderer;
			D3D12Device(IDXGIFactory4* factory, const SharedFileSystemPtr& fs);
			~D3D12Device()override;
			void ClearRenderTarget(const SharedRenderTargetPtr& rt, const ColorF& color, const RectIList* rects=nullptr)override;
			void ClearDepthStencilBuffer(const SharedDepthStencilBufferPtr& buffer, DepthStencilClearFlags flags, float depth, std::uint8_t stencil, const RectIList* rects = nullptr)override;
			SharedVertexBufferPtr CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor)override;
			SharedIndexBufferPtr CreateIndexBuffer(std::uint32_t bufferSize, IndexType type)override;
			ID3D12CommandQueue* GetCommandQueue()const { return mCommandQueue.Get(); }
			ID3D12GraphicsCommandList* GetGraphicsCommandList();
			SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap)override;
			void ApplyPipelineState(const PipelineState& state)override;
			void BindGPUBuffer(std::uint8_t slot, const SharedGPUBufferPtr& buffer)override;
			void DrawVertex(const std::size_t vertexCountPerInstance, const std::size_t instanceCount, const std::size_t firstVertexIndex, const std::size_t instanceDataOffset)override;
			void DrawIndexed(const std::size_t indexCountPerInstance, const std::size_t instanceCount, const std::size_t firstIndex, const std::size_t indexDataOffset, const std::size_t instanceDataOffset)override;
			void BeginFrame(const std::size_t frameResourceIndex)override;
			void EndFrame(const std::size_t frameResourceIndex)override;
			void GetAllCommandLists(std::size_t frameResourceIndex, container::vector<ID3D12CommandList*>& lists);
			void ApplyRenderTargets(const container::vector<SharedRenderTargetPtr>& renderTargets, const SharedDepthStencilBufferPtr& dsBuffer)override;
			//native device method wrapper start
			ComPtr<ID3D12Resource> CreateCommittedResource(
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
			UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType);
			ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap( const D3D12_DESCRIPTOR_HEAP_DESC *pDescriptorHeapDesc);
			ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type);
			ComPtr<ID3D12CommandList> CreateCommandList( 
				UINT nodeMask,
				D3D12_COMMAND_LIST_TYPE type,
				ID3D12CommandAllocator *pCommandAllocator,
				ID3D12PipelineState *pInitialState);
			HRESULT CheckFeatureSupport( D3D12_FEATURE Feature, void *pFeatureSupportData, UINT FeatureSupportDataSize);
			ComPtr<ID3D12Fence> CreateFence( UINT64 InitialValue, D3D12_FENCE_FLAGS Flags);
			//native device method wrapper end
		private:
			struct PipelineStateRootSignature
			{
				ComPtr<ID3D12PipelineState> pipelineState;
				ComPtr<ID3D12RootSignature> rootSignature;
			};
			struct ShaderResourceHandle
			{
				explicit ShaderResourceHandle(const D3D12RootBoundResource& res) : resource(res) {}
				D3D12RootBoundResource resource;
				D3D12_GPU_DESCRIPTOR_HANDLE handle;
			};
			//if parameter pState is nullptr,this method will create a default pipeline state
			using PipelineCacheMap = container::unordered_map<std::size_t, PipelineStateRootSignature>;
			using RootSignatureMap = container::unordered_map<std::size_t, ComPtr<ID3D12RootSignature>>;
			void CreateNativeDevice(IDXGIFactory4* factory);
			void ApplyRasterizerState(const RasterizerState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyBlendStates(const std::uint8_t firstRTIndex, const BlendState* states, const std::uint8_t stateCount, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyDepthStencilState(const DepthStencilState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyViewports(const RectFList& vp, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyScissorRects(const RectFList& scissorRects, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyShader(IShader* pShader, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void UpdatePSOInputLayout(const VertexInputLayout *inputLayouts, std::uint8_t  layoutCount, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			ComPtr<ID3D12RootSignature> GetRootSignature(const container::vector<IShader*>& shaders);
			PipelineStateRootSignature CreateAndCachePipelineState(const PipelineState& pState, std::size_t hashValue);
			void BindShaderResources(const PipelineState& state);
			//Analyze shader root resources.
			//Returns number of constant buffers used by this shader
			std::size_t AnalyzeShaderRootResources(IShader *pShader, container::unordered_map<ShaderType, container::vector<ShaderResourceHandle>>& resourceHandles);
			ComPtr<ID3D12Device> mDevice;
			ComPtr<ID3D12CommandQueue> mCommandQueue;
			PipelineCacheMap mPipelineCache;
			RootSignatureMap mRootSignatures;
			Foundation::ThreadLocalSingleton<D3D12FrameResources> mFrameResources[RENDER_FRAME_COUNT];
		};
	}
}
