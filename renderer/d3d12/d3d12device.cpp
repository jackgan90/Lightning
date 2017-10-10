#include <d3dx12.h>
#include "rendererfactory.h"
#include "d3d12device.h"
#include "d3d12rendertarget.h"
#include "d3d12swapchain.h"
#include "d3d12pipelinestateobject.h"
#include "d3d12blendstate.h"
#include "d3d12depthstencilstate.h"
#include "d3d12rasterizerstate.h"
#include "d3d12shader.h"
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
		D3D12Device::D3D12Device(ComPtr<ID3D12Device> pDevice, D3D12RenderContext* pContext)
		{
			m_context = pContext;
			m_device = pDevice;
			D3D12_COMMAND_QUEUE_DESC desc = {};
			//TODO : here may cause the object he in inconsistent status due to the failure of command queue creation
			HRESULT hr = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
			if (FAILED(hr))
			{
				logger.Log(LogLevel::Error, "Failed to create command queue!");
			}
			const EngineConfig& config = ConfigManager::Instance()->GetConfig();
			for (int i = 0; i < config.SwapChainBufferCount; i++)
			{
				ComPtr<ID3D12CommandAllocator> allocator;
				hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
				if (FAILED(hr))
				{
					logger.Log(LogLevel::Error, "Failed to create command allocator!");
					return;
				}
				m_commandAllocators.push_back(allocator);
			}
			hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_commandList));
			if (FAILED(hr))
			{
				logger.Log(LogLevel::Error, "Failed to create command list!");
				return;
			}
			m_commandList->Close();
		}

		D3D12Device::~D3D12Device()
		{

		}

		void D3D12Device::ReleaseRenderResources()
		{
			m_commandList.Reset();
			//TODO : command allocator should be reset only after the previous frame has finished execution
			for (auto allocator : m_commandAllocators)
				allocator.Reset();
			m_commandAllocators.clear();
			m_commandQueue.Reset();
			m_device.Reset();
			RendererFactory<IShaderManager>::Instance()->Get()->ReleaseRenderResources();
			RendererFactory<IShaderManager>::Instance()->Finalize();
		}

		void D3D12Device::ClearRenderTarget(const RenderTargetPtr& rt, const ColorF& color, const RectI* pRects, const int rectCount)
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
				//TODO here may new a few small objects ,consider use memory allocator to boost performance
				D3D12_RECT* d3dRect = new D3D12_RECT[rectCount];
				for (int i = 0; i < rectCount; i++)
				{
					d3dRect[i].left = pRects[i].left();
					d3dRect[i].right = pRects[i].right();
					d3dRect[i].top = pRects[i].top();
					d3dRect[i].bottom = pRects[i].bottom();
				}
				m_commandList->ClearRenderTargetView(rtvHandle, clearColor, rectCount, d3dRect);
				delete[] d3dRect;
			}
			else
			{
				m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
			}

			if (rt->IsSwapChainRenderTarget())
				m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRenderTarget.Get(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		}

		BlendStatePtr D3D12Device::CreateBlendState()
		{
			return BlendStatePtr(new D3D12BlendState());
		}

		DepthStencilStatePtr D3D12Device::CreateDepthStencilState()
		{
			return DepthStencilStatePtr(new D3D12DepthStencilState());
		}


		PipelineStateObjectPtr D3D12Device::CreatePipelineStateObject()
		{
			return PipelineStateObjectPtr(new D3D12PipelineStateObject());
		}

		VertexBufferPtr D3D12Device::CreateVertexBuffer()
		{
			//TODO : replace with d3d12 vertex buffer
			return VertexBufferPtr();
		}

		ShaderPtr D3D12Device::CreateShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap)
		{
			return RendererFactory<IShaderManager>::Instance()->Create()->GetShader(type, shaderName, defineMap);
		}

		RasterizerStatePtr D3D12Device::CreateRasterizerState()
		{
			return RasterizerStatePtr(new D3D12RasterizerState());
		}


	}
}