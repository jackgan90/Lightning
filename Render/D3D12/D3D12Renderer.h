#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#ifndef NDEBUG
#include <dxgidebug.h>
#endif
#include "IWindow.h"
#include "IRenderFence.h"
#include "Renderer.h"
#include "IFileSystem.h"
#include "D3D12SwapChain.h"
#include "D3D12DepthStencilBuffer.h"
#include "D3D12CommandEncoder.h"
#include "D3D12ShaderGroup.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		//Thread safe
		class D3D12Renderer : public Renderer
		{
		public:
			D3D12Renderer(Window::IWindow* window);
			~D3D12Renderer()override;
			float GetNDCNearPlane()const override { return 0.0f; }
			void ShutDown()override;
			void Start()override;
			void ClearRenderTarget(IRenderTarget* renderTarget, const ColorF& color, 
				const RectI* rects=nullptr, std::size_t rectCount = 0)override;
			void ClearDepthStencilBuffer(IDepthStencilBuffer* buffer, DepthStencilClearFlags flags, float depth, std::uint8_t stencil, 
				const RectI* rects = nullptr, std::size_t rectCount = 0)override;
			void ApplyRenderTargets(const IRenderTarget*const * renderTargets, std::size_t renderTargetCount, IDepthStencilBuffer* dsBuffer)override;
			void ApplyPipelineState(const PipelineState& state)override;
			void ApplyViewports(const Viewport* viewports, std::size_t viewportCount)override;
			void ApplyScissorRects(const ScissorRect* scissorRects, std::size_t scissorRectCount)override;
			void BindGPUBuffer(std::size_t slot, IGPUBuffer* buffer)override;
			void Draw(const DrawParam& param)override;
			//For debug
			void ReportLiveObjects()const;
			ID3D12CommandQueue* GetCommandQueue();
			ID3D12GraphicsCommandList* GetGraphicsCommandList();
		protected:
			void OnFrameBegin()override;
			void OnFrameUpdate()override;
			void OnFrameEnd()override;
			void ResizeDepthStencilBuffer(IDepthStencilBuffer* depthStencilBuffer, std::size_t width, std::size_t height)override;
			IRenderFence* CreateRenderFence()override;
			Device* CreateDevice()override;
			SwapChain* CreateSwapChain()override;
			RenderPass* CreateRenderPass(RenderPassType type)override;
		private:
			struct PipelineCacheObject
			{
				ComPtr<ID3D12PipelineState> pipelineState;
				std::shared_ptr<D3D12ShaderGroup> shaderGroup;
			};
			using PipelineCacheMap = std::unordered_map<std::size_t, PipelineCacheObject>;

			PipelineCacheObject CreateAndCachePipelineState(const PipelineState& pState, std::size_t hashValue);
			void ApplyRasterizerState(const RasterizerState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyBlendStates(const std::vector<RenderTargetBlendState>& states, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyDepthStencilState(const DepthStencilState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyShader(const std::shared_ptr<IShader>& pShader, D3D12ShaderGroup* shaderGroup, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void UpdatePSOInputLayout(const std::vector<VertexInputLayout>& inputLayouts, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

			ComPtr<IDXGIFactory4> mDXGIFactory;
			ComPtr<ID3D12CommandQueue> mCommandQueue;
			Foundation::ThreadLocalObject<D3D12CommandEncoder> mCmdEncoders[RENDER_FRAME_COUNT];
			PipelineCacheMap mPipelineCache;
#ifndef NDEBUG
			ComPtr<ID3D12Debug> mD3D12Debug;
			ComPtr<ID3D12Debug1> mD3D12Debug1;
			ComPtr<IDXGIDebug> mDXGIDebug;
			void EnableDebugLayer();
			void InitDXGIDebug();
#endif
		};
	}
}
