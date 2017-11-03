#pragma once
#include <d3d12.h>
#include <wrl\client.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "device.h"
#include "stackallocator.h"
#include "filesystem.h"
#include "d3d12shadermanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::SharedFileSystemPtr;
		class LIGHTNINGGE_RENDERER_API D3D12Device : public Device
		{
		public:
			friend class D3D12Renderer;
			D3D12Device(ComPtr<ID3D12Device> pDevice, const SharedFileSystemPtr& fs);
			~D3D12Device()override;
			void ClearRenderTarget(IRenderTarget* rt, const ColorF& color, const RenderIRects* rects=nullptr)override;
			SharedVertexBufferPtr CreateVertexBuffer()override;
			ComPtr<ID3D12Device> GetNativeDevice()const { return m_device; }
			SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap)override;
			void ApplyRasterizerState(const RasterizerState& state)override;
			void ApplyBlendState(const BlendState& state)override;
			void ApplyDepthStencilState(const DepthStencilState& state)override;
			void ApplyPipelineState(const PipelineState& state)override;
			void ApplyViewports(const RenderViewports& vp)override;
			void ApplyScissorRects(const RenderScissorRects& scissorRects)override;
		private:
			//if parameter pState is nullptr,this method will create a default pipeline state
			void SetUpDefaultPipelineState();
			ComPtr<ID3D12PipelineState> CreateAndCachePipelineState(const PipelineState& pState, std::size_t hashValue);
			SharedFileSystemPtr m_fs;
			ComPtr<ID3D12Device> m_device;
			ComPtr<ID3D12CommandQueue> m_commandQueue;
			std::vector<ComPtr<ID3D12CommandAllocator>> m_commandAllocators;
			D3D12_INPUT_ELEMENT_DESC* m_inputElements;
			size_t m_inputElementCount;
			ComPtr<ID3D12GraphicsCommandList> m_commandList;
			ComPtr<ID3D12PipelineState> m_pipelineState;
			D3D12_VIEWPORT m_viewport;
			std::unique_ptr<IMemoryAllocator> m_smallObjAllocator;
			D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipelineDesc;
			std::unordered_map<std::size_t, ComPtr<ID3D12PipelineState>> m_pipelineCache;
		};
	}
}