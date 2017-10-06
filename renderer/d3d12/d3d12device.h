#pragma once
#include <d3d12.h>
#include <wrl\client.h>
#include <vector>
#include <memory>
#include "idevice.h"
#include "d3d12rendercontext.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12Device : public IDevice
		{
		public:
			friend class D3D12RenderContext;
			friend class D3D12Renderer;
			friend class D3D12SwapChain;
			D3D12Device(ComPtr<ID3D12Device> pDevice, D3D12RenderContext* pContext);
			~D3D12Device()override;
			void ClearRenderTarget(const RenderTargetPtr& rt, const ColorF& color, const RectI* pRects = nullptr, const int rectCount = 0)override;
			void ReleaseRenderResources()override;
			BlendStatePtr CreateBlendState()override;
			DepthStencilStatePtr CreateDepthStencilState()override;
			PipelineStateObjectPtr CreatePipelineStateObject()override;
			VertexBufferPtr CreateVertexBuffer()override;
			ShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap)override;
			RasterizerStatePtr CreateRasterizerState()override;
		private:
			ComPtr<ID3D12Device> m_device;
			ComPtr<ID3D12CommandQueue> m_commandQueue;
			std::vector<ComPtr<ID3D12CommandAllocator>> m_commandAllocators;
			ComPtr<ID3D12GraphicsCommandList> m_commandList;
			D3D12RenderContext* m_context;
		};
	}
}