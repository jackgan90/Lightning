#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <memory>
#include "container.h"
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
			ID3D12Device* GetNative()const { return mDevice.Get(); }
			ID3D12CommandQueue* GetCommandQueue()const { return mCommandQueue.Get(); }
			ID3D12GraphicsCommandList* GetGraphicsCommandList()const { return mCommandList.Get(); }
			SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap)override;
			void ApplyPipelineState(const PipelineState& state)override;
			void BindGPUBuffer(std::uint8_t slot, const SharedGPUBufferPtr& buffer)override;
			void DrawVertex(const std::size_t vertexCountPerInstance, const std::size_t instanceCount, const std::size_t firstVertexIndex, const std::size_t instanceDataOffset)override;
			void DrawIndexed(const std::size_t indexCountPerInstance, const std::size_t instanceCount, const std::size_t firstIndex, const std::size_t indexDataOffset, const std::size_t instanceDataOffset)override;
			void BeginFrame(const std::size_t frameResourceIndex)override;
			void ApplyRenderTargets(const container::vector<SharedRenderTargetPtr>& renderTargets, const SharedDepthStencilBufferPtr& dsBuffer)override;
		private:
			struct FrameResource
			{
				ComPtr<ID3D12CommandAllocator> commandAllocator;

				void Release(bool perFrame)
				{
					commandAllocator->Reset();
					if (!perFrame)
					{
						commandAllocator.Reset();
					}
				}
			};
			struct PipelineStateRootSignature
			{
				ComPtr<ID3D12PipelineState> pipelineState;
				ComPtr<ID3D12RootSignature> rootSignature;
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
			void ExtractShaderDescriptorHeaps(container::vector<ID3D12DescriptorHeap*>& heaps, const PipelineState& state);
			void ExtractShaderDescriptorHeaps(IShader* pShader, container::vector<ID3D12DescriptorHeap*>& heaps);
			void BindShaderResources(const PipelineState& state);
			void BindShaderResources(IShader* pShader, UINT rootParameterIndex);
			SharedFileSystemPtr mFs;
			ComPtr<ID3D12Device> mDevice;
			ComPtr<ID3D12CommandQueue> mCommandQueue;
			ComPtr<ID3D12GraphicsCommandList> mCommandList;
			PipelineCacheMap mPipelineCache;
			RootSignatureMap mRootSignatures;
			SharedDepthStencilBufferPtr mCurrentDSBuffer;
			FrameResource mFrameResources[RENDER_FRAME_COUNT];
		};
	}
}
