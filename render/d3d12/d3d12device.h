#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "stackallocator.h"
#include "device.h"
#include "stackallocator.h"
#include "filesystem.h"
#include "d3d12shadermanager.h"

namespace LightningGE
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::SharedFileSystemPtr;
		using Foundation::StackAllocator;
		class LIGHTNINGGE_RENDER_API D3D12Device : public Device
		{
		public:
			friend class D3D12Renderer;
			D3D12Device(IDXGIFactory4* factory, const SharedFileSystemPtr& fs);
			~D3D12Device()override;
			void ClearRenderTarget(const SharedRenderTargetPtr& rt, const ColorF& color, const RectIList* rects=nullptr)override;
			void ClearDepthStencilBuffer(const SharedDepthStencilBufferPtr& buffer, DepthStencilClearFlags flags, float depth, std::uint8_t stencil, const RectIList* rects = nullptr)override;
			SharedVertexBufferPtr CreateVertexBuffer()override;
			ID3D12Device* GetNative()const { return m_device.Get(); }
			ID3D12CommandQueue* GetCommandQueue()const { return m_commandQueue.Get(); }
			ID3D12GraphicsCommandList* GetGraphicsCommandList()const { return m_commandList.Get(); }
			SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap)override;
			void ApplyPipelineState(const PipelineState& state)override;
			void CommitGPUBuffer(const GPUBuffer* pBuffer)override;
			void BindGPUBuffers(std::uint8_t startSlot, const std::vector<GPUBuffer*>& buffers)override;
			void DrawVertex(const std::size_t vertexCountPerInstance, const std::size_t instanceCount, const std::size_t firstVertexIndex, const std::size_t instanceDataOffset)override;
			void DrawIndexed(const std::size_t indexCountPerInstance, const std::size_t instanceCount, const std::size_t firstIndex, const std::size_t indexDataOffset, const std::size_t instanceDataOffset)override;
			void BeginFrame(const std::size_t frameResourceIndex);
		protected:
			void ApplyRasterizerState(const RasterizerState& state)override;
			void ApplyBlendStates(const std::uint8_t firstRTIndex, const BlendState* states, const std::uint8_t stateCount)override;
			void ApplyDepthStencilState(const DepthStencilState& state)override;
			void ApplyViewports(const RectFList& vp)override;
			void ApplyScissorRects(const RectFList& scissorRects)override;
			void ApplyRenderTargets(const SharedRenderTargetPtr* renderTargets, const std::uint8_t targetCount, const SharedDepthStencilBufferPtr& dsBuffer)override;
		private:
			struct GPUBufferCommit
			{
				ComPtr<ID3D12Resource> uploadHeap;
				ComPtr<ID3D12Resource> defaultHeap;
				GPUBufferType type;
				union
				{
					D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
					D3D12_INDEX_BUFFER_VIEW indexBufferView;
				};
			};

			struct FrameResource
			{
				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[MAX_RENDER_TARGET_COUNT];
				//The reason we don't use ComPtr here is that descriptor heaps are not released during rendering
				//so it's not necessary to hold share pointers just to prevent them from being released
				//descriptorHeaps is more like an intermediate container because descriptor heaps are not released
				//so the only purpose of it is to pass arguments to DirectX API
				std::vector<ID3D12DescriptorHeap*> descriptorHeaps;
				std::unordered_map<IDepthStencilBuffer*, SharedDepthStencilBufferPtr> depthStencilBuffers;
				std::unordered_map<IRenderTarget*, SharedRenderTargetPtr> renderTargets;
				D3D12_VERTEX_BUFFER_VIEW vbViews[MAX_GEOMETRY_BUFFER_COUNT];
				ComPtr<ID3D12CommandAllocator> commandAllocator;

				void Release(bool perFrame)
				{
					descriptorHeaps.clear();
					depthStencilBuffers.clear();
					renderTargets.clear();
					if (!perFrame)
					{
						commandAllocator.Reset();
					}
					else
					{
						commandAllocator->Reset();
					}
				}
			};
			//if parameter pState is nullptr,this method will create a default pipeline state
			using PipelineCacheMap = std::unordered_map<std::size_t, ComPtr<ID3D12PipelineState>>;
			using RootSignatureMap = std::unordered_map<std::size_t, ComPtr<ID3D12RootSignature>>;
			void CreateNativeDevice(IDXGIFactory4* factory);
			void ApplyShader(IShader* pShader);
			void UpdatePSOInputLayout(const std::vector<VertexInputLayout>& inputLayouts);
			void SetUpDefaultPipelineStates();
			ComPtr<ID3D12RootSignature> GetRootSignature(const std::vector<IShader*>& shaders);
			ComPtr<ID3D12PipelineState> CreateAndCachePipelineState(const PipelineState& pState, std::size_t hashValue);
			void ExtractShaderDescriptorHeaps();
			void ExtractShaderDescriptorHeaps(IShader* pShader);
			void BindShaderResources();
			void BindShaderResources(IShader* pShader, UINT rootParameterIndex);
			SharedFileSystemPtr m_fs;
			ComPtr<ID3D12Device> m_device;
			ComPtr<ID3D12CommandQueue> m_commandQueue;
			ComPtr<ID3D12GraphicsCommandList> m_commandList;
			ComPtr<ID3D12PipelineState> m_d3d12PipelineState;
			D3D12_VIEWPORT m_viewport;
			std::unique_ptr<IMemoryAllocator> m_smallObjAllocator;
			D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipelineDesc;
			PipelineCacheMap m_pipelineCache;
			RootSignatureMap m_rootSignatures;
			D3D12_INPUT_ELEMENT_DESC* m_pInputElementDesc;
			std::unordered_map<const GPUBuffer*, GPUBufferCommit> m_bufferCommitMap;
			//depth stencil buffer is a resource of Renderer and will be kept alive during rendering cycle.So the availability of
			//this resource is ensured by Renderer.Device need not to use shared ptr to keep it valid 
			SharedDepthStencilBufferPtr m_currentDSBuffer;
			std::uint8_t m_frameResourceIndex;
			FrameResource m_frameResources[RENDER_FRAME_COUNT];
		};
	}
}