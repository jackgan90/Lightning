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
#include "D3D12DescriptorHeapManager.h"
#include "D3D12SwapChain.h"
#include "D3D12RenderTargetManager.h"
#include "D3D12DepthStencilBuffer.h"
#include "D3D12CommandEncoder.h"

#ifndef NDEBUG
#define REPORT_LIVE_OBJECTS if(mDXGIDebug) {mDXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);}
#else
#define REPORT_LIVE_OBJECTS
#endif

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
			void ApplyRenderTargets(const IRenderTarget*const * renderTargets, std::uint8_t renderTargetCount, IDepthStencilBuffer* dsBuffer)override;
			void ApplyPipelineState(const PipelineState& state)override;
			void BindGPUBuffer(std::uint8_t slot, IGPUBuffer* buffer)override;
			void Draw(const DrawParam& param)override;
			ID3D12CommandQueue* GetCommandQueue();
			ID3D12GraphicsCommandList* GetGraphicsCommandList();
		protected:
			void OnFrameBegin()override;
			void OnFrameUpdate()override;
			void OnFrameEnd()override;
			IRenderFence* CreateRenderFence()override;
			IDevice* CreateDevice()override;
			ISwapChain* CreateSwapChain()override;
			IDepthStencilBuffer* CreateDepthStencilBuffer(std::uint32_t width, std::uint32_t height)override;
			RenderPass* CreateRenderPass(RenderPassType type)override;
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
			using PipelineCacheMap = Container::UnorderedMap<std::size_t, PipelineStateRootSignature>;
			using RootSignatureMap = Container::UnorderedMap<std::size_t, ComPtr<ID3D12RootSignature>>;

			ComPtr<ID3D12RootSignature> GetRootSignature(const Container::Vector<IShader*>& shaders);
			PipelineStateRootSignature CreateAndCachePipelineState(const PipelineState& pState, std::size_t hashValue);
			void BindShaderResources(const PipelineState& state);
			void ApplyRasterizerState(const RasterizerState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyBlendStates(const std::uint8_t firstRTIndex, const BlendState* states, const std::uint8_t stateCount, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyDepthStencilState(const DepthStencilState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyViewports(const RectF*const* vps, std::size_t count, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyScissorRects(const RectF*const* scissorRects, std::size_t count, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyShader(IShader* pShader, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void UpdatePSOInputLayout(const VertexInputLayout *inputLayouts, std::uint8_t  layoutCount, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			//Analyze shader root resources.
			//Returns number of constant buffers used by this shader
			std::size_t AnalyzeShaderRootResources(IShader *pShader, Container::UnorderedMap<ShaderType, Container::Vector<ShaderResourceHandle>>& resourceHandles);

			ComPtr<IDXGIFactory4> mDXGIFactory;
			ComPtr<ID3D12CommandQueue> mCommandQueue;
			Foundation::ThreadLocalSingleton<D3D12CommandEncoder> mCmdEncoders[RENDER_FRAME_COUNT];
			PipelineCacheMap mPipelineCache;
			RootSignatureMap mRootSignatures;
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
