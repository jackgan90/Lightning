#include <d3dx12.h>
//#include "rendererfactory.h"
#include "common.h"
#include "irenderer.h"
#include "d3d12device.h"
#include "d3d12rendertarget.h"
#include "d3d12swapchain.h"
#include "d3d12shader.h"
#include "d3d12typemapper.h"
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
		D3D12Device::D3D12Device(ComPtr<ID3D12Device> pDevice, const SharedFileSystemPtr& fs):m_fs(fs)
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
		}

		D3D12Device::~D3D12Device()
		{
		}

		void D3D12Device::ClearRenderTarget(const SharedRenderTargetPtr& rt, const ColorF& color, const RectI* pRects, const int rectCount)
		{
			D3D12RenderTarget *pTarget = static_cast<D3D12RenderTarget*>(rt.get());
			ComPtr<ID3D12Resource> nativeRenderTarget = pTarget->GetNative();
			if (rt->IsSwapChainRenderTarget())
				m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRenderTarget.Get(),
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

			//TODO : check gpu?
			const float clearColor[] = { color.r(), color.g(), color.b(), color.a() };
			auto rtvHandle = pTarget->GetCPUHandle();
			m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
			if (pRects && rectCount)
			{
				D3D12_RECT* d3dRect = ALLOC_ARRAY(m_smallObjAllocator, rectCount, D3D12_RECT);
				for (int i = 0; i < rectCount; i++)
				{
					d3dRect[i].left = pRects[i].left();
					d3dRect[i].right = pRects[i].right();
					d3dRect[i].top = pRects[i].top();
					d3dRect[i].bottom = pRects[i].bottom();
				}
				m_commandList->ClearRenderTargetView(rtvHandle, clearColor, rectCount, d3dRect);
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

		SharedShaderPtr D3D12Device::CreateShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap)
		{
			return m_shaderMgr->GetShader(type, shaderName, defineMap);
		}

		void D3D12Device::ApplyRasterizerState(const RasterizerState& state)
		{
			m_pipelineDesc.RasterizerState.FillMode = D3D12TypeMapper::MapFillMode(state.fillMode);
			m_pipelineDesc.RasterizerState.CullMode = D3D12TypeMapper::MapCullMode(state.cullMode);
			m_pipelineDesc.RasterizerState.FrontCounterClockwise = state.frontFaceWindingOrder == COUNTER_CLOCKWISE;
		}

		void D3D12Device::ApplyBlendState(const BlendState& state)
		{
			auto boundIndex = state.renderTarget->GetBoundIndex();
			assert(boundIndex >= 0);
			D3D12_RENDER_TARGET_BLEND_DESC desc = m_pipelineDesc.BlendState.RenderTarget[boundIndex];
			desc.BlendEnable = state.enable;
			desc.BlendOp = D3D12TypeMapper::MapBlendOp(state.colorOp);
			desc.BlendOpAlpha = D3D12TypeMapper::MapBlendOp(state.alphaOp);
			desc.SrcBlend = D3D12TypeMapper::MapBlendFactor(state.srcColorFactor);
			desc.SrcBlendAlpha = D3D12TypeMapper::MapBlendFactor(state.srcAlphaFactor);
			desc.DestBlend = D3D12TypeMapper::MapBlendFactor(state.destColorFactor);
			desc.DestBlendAlpha = D3D12TypeMapper::MapBlendFactor(state.destAlphaFactor);
		}

		void D3D12Device::ApplyDepthStencilState(const DepthStencilState& state)
		{
			D3D12_DEPTH_STENCIL_DESC dsDesc = m_pipelineDesc.DepthStencilState;
			dsDesc.DepthEnable = state.depthTestEnable;
			dsDesc.DepthFunc = D3D12TypeMapper::MapCmpFunc(state.depthCmpFunc);
			dsDesc.DepthWriteMask = state.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
			dsDesc.StencilEnable = state.stencilEnable;
			dsDesc.StencilReadMask = state.stencilReadMask;
			dsDesc.StencilWriteMask = state.stencilWriteMask;

			dsDesc.FrontFace.StencilFunc = D3D12TypeMapper::MapCmpFunc(state.frontFace.cmpFunc);
			dsDesc.FrontFace.StencilPassOp = D3D12TypeMapper::MapStencilOp(state.frontFace.passOp);
			dsDesc.FrontFace.StencilFailOp = D3D12TypeMapper::MapStencilOp(state.frontFace.failOp);
			dsDesc.FrontFace.StencilDepthFailOp = D3D12TypeMapper::MapStencilOp(state.frontFace.depthFailOp);

			dsDesc.BackFace.StencilFunc = D3D12TypeMapper::MapCmpFunc(state.backFace.cmpFunc);
			dsDesc.BackFace.StencilPassOp = D3D12TypeMapper::MapStencilOp(state.backFace.passOp);
			dsDesc.BackFace.StencilFailOp = D3D12TypeMapper::MapStencilOp(state.backFace.failOp);
			dsDesc.BackFace.StencilDepthFailOp = D3D12TypeMapper::MapStencilOp(state.backFace.depthFailOp);
		}

		void D3D12Device::ApplyPipelineState(const PipelineState& state)
		{
			ApplyRasterizerState(state.rasterizerState);
			ApplyBlendState(state.blendState);
			ApplyDepthStencilState(state.depthStencilState);
		}
	}
}