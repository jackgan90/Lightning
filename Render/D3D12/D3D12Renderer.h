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
#include "D3D12DepthStencilBuffer.h"
#include "D3D12CommandEncoder.h"

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
			INTERFACECALL ~D3D12Renderer()override;
			float INTERFACECALL GetNDCNearPlane()const override { return 0.0f; }
			void INTERFACECALL ShutDown()override;
			void INTERFACECALL Start()override;
			void INTERFACECALL ClearRenderTarget(IRenderTarget* renderTarget, const ColorF& color, 
				const RectI* rects=nullptr, std::size_t rectCount = 0)override;
			void INTERFACECALL ClearDepthStencilBuffer(IDepthStencilBuffer* buffer, DepthStencilClearFlags flags, float depth, std::uint8_t stencil, 
				const RectI* rects = nullptr, std::size_t rectCount = 0)override;
			void INTERFACECALL ApplyRenderTargets(const IRenderTarget*const * renderTargets, std::size_t renderTargetCount, IDepthStencilBuffer* dsBuffer)override;
			void INTERFACECALL ApplyPipelineState(const PipelineState& state)override;
			void INTERFACECALL ApplyViewports(const Viewport* viewports, std::size_t viewportCount)override;
			void INTERFACECALL ApplyScissorRects(const ScissorRect* scissorRects, std::size_t scissorRectCount)override;
			void INTERFACECALL BindGPUBuffer(std::size_t slot, IGPUBuffer* buffer)override;
			void INTERFACECALL Draw(const DrawParam& param)override;
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
			struct PipelineStateRootSignature
			{
				ComPtr<ID3D12PipelineState> pipelineState;
				ComPtr<ID3D12RootSignature> rootSignature;
			};
			//enumerate_thread_specific cannot return array objects.Wrap array in a class.
			struct ShaderRootBoundResources
			{
				Container::Vector<D3D12RootBoundResource> Array[std::size_t(ShaderType::SHADER_TYPE_NUM)];
				Container::Vector<D3D12RootBoundResource>& At(ShaderType shaderType)
				{
					return Array[static_cast<std::size_t>(shaderType)];
				}
			};
			using PipelineCacheMap = Container::UnorderedMap<std::size_t, PipelineStateRootSignature>;
			using RootSignatureMap = Container::UnorderedMap<std::size_t, ComPtr<ID3D12RootSignature>>;

			ComPtr<ID3D12RootSignature> GetRootSignature(const Container::Vector<IShader*>& shaders);
			PipelineStateRootSignature CreateAndCachePipelineState(const PipelineState& pState, std::size_t hashValue);
			void BindShaderResources(const PipelineState& state);
			void ApplyRasterizerState(const RasterizerState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyBlendStates(std::size_t firstRTIndex, const BlendState* states, std::size_t stateCount, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyDepthStencilState(const DepthStencilState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void ApplyShader(IShader* pShader, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			void UpdatePSOInputLayout(const VertexInputLayout *inputLayouts, std::size_t layoutCount, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			//Analyze shader root resources.
			//Returns number of constant buffers used by this shader
			std::size_t AnalyzeShaderRootResources(IShader *pShader, Container::Vector<D3D12RootBoundResource>& resourceHandles);

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
