#pragma once
#include <d3d12.h>
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
			D3D12Device(const ComPtr<ID3D12Device>& pDevice, const SharedFileSystemPtr& fs);
			~D3D12Device()override;
			void ClearRenderTarget(IRenderTarget* rt, const ColorF& color, const RectIList* rects=nullptr)override;
			SharedVertexBufferPtr CreateVertexBuffer()override;
			ID3D12Device* GetNative()const { return m_device.Get(); }
			ID3D12CommandQueue* GetCommandQueue()const { return m_commandQueue.Get(); }
			ID3D12GraphicsCommandList* GetGraphicsCommandList()const { return m_commandList.Get(); }
			SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap)override;
			void ApplyRasterizerState(const RasterizerState& state)override;
			void ApplyBlendState(const BlendState& state)override;
			void ApplyDepthStencilState(const DepthStencilState& state)override;
			void ApplyPipelineState(const PipelineState& state)override;
			void ApplyViewports(const RectFList& vp)override;
			void ApplyScissorRects(const RectFList& scissorRects)override;
			void ApplyRenderTargets(const RenderTargetList& renderTargets, const IDepthStencilBuffer* dsBuffer)override;
			void CommitGPUBuffer(const GPUBuffer* pBuffer)override;
			void BeginFrame(const UINT backBufferIndex);
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
			//if parameter pState is nullptr,this method will create a default pipeline state
			using PipelineCacheMap = std::unordered_map<std::size_t, ComPtr<ID3D12PipelineState>>;
			using RootSignatureMap = std::unordered_map<std::size_t, ComPtr<ID3D12RootSignature>>;
			void ApplyShader(IShader* pShader);
			void UpdatePSOInputLayout(const VertexComponentBoundMap& attributes);
			void SetUpDefaultPipelineStates();
			ComPtr<ID3D12RootSignature> GetRootSignature(const std::vector<IShader*>& shaders);
			ComPtr<ID3D12PipelineState> CreateAndCachePipelineState(const PipelineState& pState, std::size_t hashValue);
			void BindAllShaderResources();
			void BindShaderResources(IShader* pShader, UINT rootParameterIndex);
			SharedFileSystemPtr m_fs;
			ComPtr<ID3D12Device> m_device;
			ComPtr<ID3D12CommandQueue> m_commandQueue;
			std::vector<ComPtr<ID3D12CommandAllocator>> m_commandAllocators;
			size_t m_inputElementCount;
			ComPtr<ID3D12GraphicsCommandList> m_commandList;
			ComPtr<ID3D12PipelineState> m_d3d12PipelineState;
			D3D12_VIEWPORT m_viewport;
			std::unique_ptr<IMemoryAllocator> m_smallObjAllocator;
			D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipelineDesc;
			PipelineCacheMap m_pipelineCache;
			RootSignatureMap m_rootSignatures;
			D3D12_INPUT_ELEMENT_DESC* m_pInputElementDesc;
			std::unordered_map<const GPUBuffer*, GPUBufferCommit> m_bufferCommitMap;
		};
	}
}