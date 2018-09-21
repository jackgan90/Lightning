#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "device.h"
#include "stackallocator.h"
#include "filesystem.h"
#include "d3d12shadermanager.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::SharedFileSystemPtr;
		using Foundation::StackAllocator;
		class LIGHTNING_RENDER_API D3D12Device : public Device
		{
		public:
			friend class D3D12Renderer;
			D3D12Device(IDXGIFactory4* factory, const SharedFileSystemPtr& fs);
			~D3D12Device()override;
			void ClearRenderTarget(const SharedRenderTargetPtr& rt, const ColorF& color, const RectIList* rects=nullptr)override;
			void ClearDepthStencilBuffer(const SharedDepthStencilBufferPtr& buffer, DepthStencilClearFlags flags, float depth, std::uint8_t stencil, const RectIList* rects = nullptr)override;
			SharedVertexBufferPtr CreateVertexBuffer(std::uint32_t bufferSize, const std::vector<VertexComponent>& components)override;
			SharedIndexBufferPtr CreateIndexBuffer(std::uint32_t bufferSize, IndexType type)override;
			ID3D12Device* GetNative()const { return mDevice.Get(); }
			ID3D12CommandQueue* GetCommandQueue()const { return mCommandQueue.Get(); }
			ID3D12GraphicsCommandList* GetGraphicsCommandList()const { return mCommandList.Get(); }
			SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap)override;
			void ApplyPipelineState(const PipelineState& state)override;
			void CommitGPUBuffer(const SharedGPUBufferPtr& pBuffer)override;
			void BindGPUBuffers(std::uint8_t startSlot, const std::vector<SharedGPUBufferPtr>& buffers)override;
			void DrawVertex(const std::size_t vertexCountPerInstance, const std::size_t instanceCount, const std::size_t firstVertexIndex, const std::size_t instanceDataOffset)override;
			void DrawIndexed(const std::size_t indexCountPerInstance, const std::size_t instanceCount, const std::size_t firstIndex, const std::size_t indexDataOffset, const std::size_t instanceDataOffset)override;
			void BeginFrame(const std::size_t frameResourceIndex)override;
		protected:
			void ApplyRasterizerState(const RasterizerState& state)override;
			void ApplyBlendStates(const std::uint8_t firstRTIndex, const BlendState* states, const std::uint8_t stateCount)override;
			void ApplyDepthStencilState(const DepthStencilState& state)override;
			void ApplyViewports(const RectFList& vp)override;
			void ApplyScissorRects(const RectFList& scissorRects)override;
			void ApplyRenderTargets(const SharedRenderTargetPtr* renderTargets, const std::uint8_t targetCount, const SharedDepthStencilBufferPtr& dsBuffer)override;
		private:
			struct FrameResource
			{
				//The reason we don't use ComPtr here is that descriptor heaps are not released during rendering
				//so it's not necessary to hold share pointers just to prevent them from being released
				//descriptorHeaps is more like an intermediate container because descriptor heaps are not released
				//so the only purpose of it is to pass arguments to DirectX API
				std::vector<ID3D12DescriptorHeap*> descriptorHeaps;
				std::unordered_map<IDepthStencilBuffer*, SharedDepthStencilBufferPtr> depthStencilBuffers;
				std::unordered_map<IRenderTarget*, SharedRenderTargetPtr> renderTargets;
				std::unordered_map<IGPUBuffer*, SharedGPUBufferPtr> buffers;
				ComPtr<ID3D12CommandAllocator> commandAllocator;

				void Release(bool perFrame)
				{
					descriptorHeaps.clear();
					depthStencilBuffers.clear();
					renderTargets.clear();
					buffers.clear();
					commandAllocator->Reset();
					if (!perFrame)
					{
						commandAllocator.Reset();
					}
				}
			};
			//if parameter pState is nullptr,this method will create a default pipeline state
			using PipelineCacheMap = std::unordered_map<std::size_t, ComPtr<ID3D12PipelineState>>;
			using RootSignatureMap = std::unordered_map<std::size_t, ComPtr<ID3D12RootSignature>>;
			void CreateNativeDevice(IDXGIFactory4* factory);
			void ApplyShader(IShader* pShader);
			void UpdatePSOInputLayout(const VertexInputLayout *inputLayouts, std::uint8_t  layoutCount);
			ComPtr<ID3D12RootSignature> GetRootSignature(const std::vector<IShader*>& shaders);
			ComPtr<ID3D12PipelineState> CreateAndCachePipelineState(const PipelineState& pState, std::size_t hashValue);
			void ExtractShaderDescriptorHeaps();
			void ExtractShaderDescriptorHeaps(IShader* pShader);
			void BindShaderResources();
			void BindShaderResources(IShader* pShader, UINT rootParameterIndex);
			void CacheResourceReference(const SharedDepthStencilBufferPtr& resource);
			void CacheResourceReference(const SharedRenderTargetPtr& resource);
			void CacheResourceReference(const SharedGPUBufferPtr& resource);
			SharedFileSystemPtr mFs;
			ComPtr<ID3D12Device> mDevice;
			ComPtr<ID3D12CommandQueue> mCommandQueue;
			ComPtr<ID3D12GraphicsCommandList> mCommandList;
			ComPtr<ID3D12PipelineState> mD3D12PipelineState;
			D3D12_GRAPHICS_PIPELINE_STATE_DESC mPipelineDesc;
			PipelineCacheMap mPipelineCache;
			RootSignatureMap mRootSignatures;
			SharedDepthStencilBufferPtr mCurrentDSBuffer;
			FrameResource mFrameResources[RENDER_FRAME_COUNT];
		};
	}
}
