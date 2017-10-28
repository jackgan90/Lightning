#include <d3dx12.h>
#include <functional>
//#include "rendererfactory.h"
#include "common.h"
#include "irenderer.h"
#include "d3d12device.h"
#include "d3d12rendertarget.h"
#include "d3d12swapchain.h"
#include "d3d12shader.h"
#include "d3d12typemapper.h"
#include "semantics.h"
#include "shadermanager.h"
#include "logger.h"
#include "configmanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Foundation::LogLevel;
		using Foundation::logger;
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		using Foundation::StackAllocator;
		D3D12Device::D3D12Device(ComPtr<ID3D12Device> pDevice, const SharedFileSystemPtr& fs)
			:Device(), m_fs(fs), m_pipelineDesc{}
		{
			m_smallObjAllocator = std::make_unique<StackAllocator<true, 16, 8192>>();
			m_device = pDevice;
			D3D12_COMMAND_QUEUE_DESC desc = {};
			HRESULT hr = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create command queue!");
			}
			const EngineConfig& config = ConfigManager::Instance()->GetConfig();
			for (size_t i = 0; i < config.SwapChainBufferCount; i++)
			{
				ComPtr<ID3D12CommandAllocator> allocator;
				hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
				if (FAILED(hr))
				{
					throw DeviceInitException("Failed to create command allocator!");
				}
				m_commandAllocators.push_back(allocator);
			}
			hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_commandList));
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create command list!");
			}
			m_commandList->Close();
			m_shaderMgr = std::make_unique<D3D12ShaderManager>(D3D12ShaderManager(fs));
			//should create first default pipeline state
			SetUpDefaultPipelineState();
		}

		D3D12Device::~D3D12Device()
		{
		}

		void D3D12Device::ClearRenderTarget(IRenderTarget* rt, const ColorF& color, const RenderIRects* rects)
		{
			D3D12RenderTarget *pTarget = static_cast<D3D12RenderTarget*>(rt);
			ComPtr<ID3D12Resource> nativeRenderTarget = pTarget->GetNative();
			if (rt->IsSwapChainRenderTarget())
				m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRenderTarget.Get(),
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

			//TODO : check gpu?
			const float clearColor[] = { color.r(), color.g(), color.b(), color.a() };
			auto rtvHandle = pTarget->GetCPUHandle();
			m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
			if (rects && !rects->empty())
			{
				D3D12_RECT* d3dRect = ALLOC_ARRAY(m_smallObjAllocator, rects->size(), D3D12_RECT);
				for (int i = 0; i < rects->size(); i++)
				{
					d3dRect[i].left = (*rects)[i].left();
					d3dRect[i].right = (*rects)[i].right();
					d3dRect[i].top = (*rects)[i].top();
					d3dRect[i].bottom = (*rects)[i].bottom();
				}
				m_commandList->ClearRenderTargetView(rtvHandle, clearColor, rects->size(), d3dRect);
				DEALLOC(m_smallObjAllocator, d3dRect);
			}
			else
			{
				m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
			}

			if (rt->IsSwapChainRenderTarget())
				m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRenderTarget.Get(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		}


		SharedVertexBufferPtr D3D12Device::CreateVertexBuffer()
		{
			//TODO : replace with d3d12 vertex buffer
			return SharedVertexBufferPtr();
		}

		void D3D12Device::ApplyRasterizerState(const RasterizerState& state)
		{
			Device::ApplyRasterizerState(state);
			D3D12_RASTERIZER_DESC* pDesc = &m_pipelineDesc.RasterizerState;
			pDesc->FillMode = D3D12TypeMapper::MapFillMode(state.fillMode);
			pDesc->CullMode = D3D12TypeMapper::MapCullMode(state.cullMode);
			pDesc->FrontCounterClockwise = state.frontFaceWindingOrder == COUNTER_CLOCKWISE;
		}

		void D3D12Device::ApplyBlendState(const BlendState& state)
		{
			Device::ApplyBlendState(state);
			auto boundIndex = state.renderTarget ? state.renderTarget->GetBoundIndex() : 0;
			assert(boundIndex >= 0);
			D3D12_RENDER_TARGET_BLEND_DESC* pDesc = &m_pipelineDesc.BlendState.RenderTarget[boundIndex];
			pDesc->BlendEnable = state.enable;
			pDesc->BlendOp = D3D12TypeMapper::MapBlendOp(state.colorOp);
			pDesc->BlendOpAlpha = D3D12TypeMapper::MapBlendOp(state.alphaOp);
			pDesc->SrcBlend = D3D12TypeMapper::MapBlendFactor(state.srcColorFactor);
			pDesc->SrcBlendAlpha = D3D12TypeMapper::MapBlendFactor(state.srcAlphaFactor);
			pDesc->DestBlend = D3D12TypeMapper::MapBlendFactor(state.destColorFactor);
			pDesc->DestBlendAlpha = D3D12TypeMapper::MapBlendFactor(state.destAlphaFactor);
		}

		void D3D12Device::ApplyDepthStencilState(const DepthStencilState& state)
		{
			Device::ApplyDepthStencilState(state);
			D3D12_DEPTH_STENCIL_DESC* pDesc = &m_pipelineDesc.DepthStencilState;
			pDesc->DepthEnable = state.depthTestEnable;
			pDesc->DepthFunc = D3D12TypeMapper::MapCmpFunc(state.depthCmpFunc);
			pDesc->DepthWriteMask = state.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
			pDesc->StencilEnable = state.stencilEnable;
			pDesc->StencilReadMask = state.stencilReadMask;
			pDesc->StencilWriteMask = state.stencilWriteMask;

			pDesc->FrontFace.StencilFunc = D3D12TypeMapper::MapCmpFunc(state.frontFace.cmpFunc);
			pDesc->FrontFace.StencilPassOp = D3D12TypeMapper::MapStencilOp(state.frontFace.passOp);
			pDesc->FrontFace.StencilFailOp = D3D12TypeMapper::MapStencilOp(state.frontFace.failOp);
			pDesc->FrontFace.StencilDepthFailOp = D3D12TypeMapper::MapStencilOp(state.frontFace.depthFailOp);

			pDesc->BackFace.StencilFunc = D3D12TypeMapper::MapCmpFunc(state.backFace.cmpFunc);
			pDesc->BackFace.StencilPassOp = D3D12TypeMapper::MapStencilOp(state.backFace.passOp);
			pDesc->BackFace.StencilFailOp = D3D12TypeMapper::MapStencilOp(state.backFace.failOp);
			pDesc->BackFace.StencilDepthFailOp = D3D12TypeMapper::MapStencilOp(state.backFace.depthFailOp);
		}

		void D3D12Device::ApplyPipelineState(const PipelineState& state)
		{
			Device::ApplyPipelineState(state);
			ComPtr<ID3D12PipelineState> pipelineState;
			auto hashValue = std::hash<PipelineState>{}(state);
			auto cachedStateObject = m_pipelineCache.find(hashValue);
			if (cachedStateObject != m_pipelineCache.end())
			{
				pipelineState = cachedStateObject->second;
			}
			else
			{
				pipelineState = CreateAndCachePipelineState(state, hashValue);
			}
			m_pipelineState = pipelineState;
			m_commandList->SetPipelineState(m_pipelineState.Get());
		}

		void D3D12Device::ApplyViewports(const RenderViewports& vp)
		{

		}

		void D3D12Device::ApplyScissorRects(const RenderScissorRects& scissorRects)
		{

		}

		ComPtr<ID3D12PipelineState> D3D12Device::CreateAndCachePipelineState(const PipelineState& state, std::size_t hashValue)
		{
			ComPtr<ID3D12PipelineState> pipelineState;
			ApplyRasterizerState(state.rasterizerState);
			ApplyBlendState(state.blendState);
			ApplyDepthStencilState(state.depthStencilState);
			auto hr = m_device->CreateGraphicsPipelineState(&m_pipelineDesc, IID_PPV_ARGS(&pipelineState));
			if (FAILED(hr))
			{
				logger.Log(LogLevel::Error, "Failed to apply pipeline state!");
				return pipelineState;
			} 
			m_pipelineCache.emplace(hashValue, pipelineState);
			return pipelineState;
		}

		void D3D12Device::SetUpDefaultPipelineState()
		{
			CreateAndCachePipelineState(m_currentPipelineState, std::hash<PipelineState>{}(m_currentPipelineState));
		}

	}
}