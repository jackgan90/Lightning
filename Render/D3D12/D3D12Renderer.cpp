#include "D3D12Renderer.h"
#include "D3D12SwapChain.h"
#include "D3D12Device.h"
#include "D3D12DescriptorHeapManager.h"
#include "D3D12RenderTarget.h"
#include "D3D12RenderFence.h"
#include "D3D12ConstantBufferManager.h"
#include "WindowsGameWindow.h"
#include "ConfigManager.h"
#include "Logger.h"
#include "Common.h"
#include "FrameMemoryAllocator.h"
#include "D3D12StatefulResourceManager.h"
#include "D3D12VertexBuffer.h"
#include "D3D12IndexBuffer.h"
#include "D3D12ForwardRenderPass.h"

namespace
{
	using Mutex = tbb::spin_mutex;
	using MutexLock = Mutex::scoped_lock;
	static Mutex mtxPipelineCache;
	static Mutex mtxRootSignature;
}


namespace Lightning
{
	namespace Render
	{
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		using Window::WindowsGameWindow;
		extern FrameMemoryAllocator g_RenderAllocator;


		D3D12Renderer::~D3D12Renderer()
		{
			//Note:we should release resources in advance to make REPORT_LIVE_OBJECTS work correctly because if we let the share pointer
			//destructor run out of the scope,we cannot trace the objects 
			//device , swap chain and depth stencil buffer are parent class's members but we still release them here because we need to track alive resources
		}

		D3D12Renderer::D3D12Renderer(Window::IWindow* window) : Renderer(window)
		{
#ifndef NDEBUG
			EnableDebugLayer();
			HRESULT hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&mDXGIFactory));
#else
			HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory));
#endif
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create DXGI factory!");
			}
			LOG_INFO("Initialize D3D12 render context succeeded!");
		}

#ifndef NDEBUG
		void D3D12Renderer::EnableDebugLayer()
		{
			auto res = ::D3D12GetDebugInterface(IID_PPV_ARGS(&mD3D12Debug));
			if (FAILED(res))
			{
				LOG_WARNING("Failed to get d3d12 debug interface!You should enable Graphics Tools optional feature!ErrorCode : {0 : x}", res);
			}
			else
			{
				mD3D12Debug->EnableDebugLayer();
				mD3D12Debug->QueryInterface(IID_PPV_ARGS(&mD3D12Debug1));
				mD3D12Debug1->SetEnableGPUBasedValidation(true);
			}
		}
#endif
		void D3D12Renderer::Start()
		{
			Renderer::Start();
#ifndef NDEBUG
			InitDXGIDebug();
#endif
			ReportLiveObjects();
		}

		void D3D12Renderer::ShutDown()
		{
			LOG_INFO("Start to clean up render resources.");
			Renderer::ShutDown();
			D3D12DescriptorHeapManager::Instance()->Clear();
			D3D12ConstantBufferManager::Instance()->Clear();
			D3D12StatefulResourceManager::Instance()->Clear();
			mCommandQueue.Reset();
			mDXGIFactory.Reset();
			mPipelineCache.clear();
			mRootSignatures.clear();
			for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				mCmdEncoders[i].for_each([](D3D12CommandEncoder& encoder) {
					encoder.Clear();
				});
			}
			ReportLiveObjects();
			LOG_INFO("Finished reporting live objects!");
		}

		ID3D12CommandQueue* D3D12Renderer::GetCommandQueue()
		{
			return mCommandQueue.Get();
		}

		ID3D12GraphicsCommandList* D3D12Renderer::GetGraphicsCommandList()
		{
			auto frameResourceIndex = GetFrameResourceIndex();
			return mCmdEncoders[frameResourceIndex]->GetCommandList();
		}

		IRenderFence* D3D12Renderer::CreateRenderFence()
		{
			auto D3DDevice = dynamic_cast<D3D12Device*>(mDevice.get());
			assert(D3DDevice != nullptr && "A D3D12Device is required.");
			return new D3D12RenderFence(D3DDevice, 0);
		}

		Device* D3D12Renderer::CreateDevice()
		{
			auto device = new D3D12Device(mDXGIFactory.Get());
			D3D12_COMMAND_QUEUE_DESC desc = {};
			mCommandQueue = device->CreateCommandQueue(&desc);
			return device;
		}

		void D3D12Renderer::ClearRenderTarget(IRenderTarget* renderTarget, const ColorF& color, 
			const RectI* rects, std::size_t rectCount)
		{
			D3D12RenderTarget *pTarget = static_cast<D3D12RenderTarget*>(renderTarget);
			assert(pTarget);
			//should check the type of the rt to transit it from previous state to render target state
			//currently just check back buffer render target
			auto commandList = GetGraphicsCommandList();
			pTarget->TransitToRTState(commandList);

			//cache render target to prevent it from being released before GPU execute ClearRenderTargetView

			const float clearColor[] = { color.r, color.g, color.b, color.a };
			auto rtvHandle = pTarget->GetCPUHandle();
			if (rects && rectCount > 0)
			{
				//TODO : This implementation is wrong.Should allocate frame memory not local memory,must fix later
				D3D12_RECT* d3dRect = g_RenderAllocator.Allocate<D3D12_RECT>(rectCount);
				for (size_t i = 0; i < rectCount; i++)
				{
					d3dRect[i].left = rects[i].left;
					d3dRect[i].right = rects[i].right();
					d3dRect[i].top = rects[i].top;
					d3dRect[i].bottom = rects[i].bottom();
				}
				commandList->ClearRenderTargetView(rtvHandle, clearColor, UINT(rectCount), d3dRect);
			}
			else
			{
				commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
			}
		}

		void D3D12Renderer::ClearDepthStencilBuffer(IDepthStencilBuffer* buffer, DepthStencilClearFlags flags, 
			float depth, std::uint8_t stencil, const RectI* rects, std::size_t rectCount)
		{
			D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH;
			if ((flags & DepthStencilClearFlags::CLEAR_DEPTH) != DepthStencilClearFlags::CLEAR_DEPTH)
			{
				clearFlags &= ~D3D12_CLEAR_FLAG_DEPTH;
			}
			if ((flags & DepthStencilClearFlags::CLEAR_STENCIL) == DepthStencilClearFlags::CLEAR_STENCIL)
			{
				clearFlags |= D3D12_CLEAR_FLAG_STENCIL;
			}
			auto commandList = GetGraphicsCommandList();
			auto d3d12DSBuffer = static_cast<D3D12DepthStencilBuffer*>(buffer);
			d3d12DSBuffer->TransitToState(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			auto dsvHandle = d3d12DSBuffer->GetCPUHandle();
			if (rects && rectCount > 0)
			{
				//TODO : This implementation is wrong.Should allocate frame memory not local memory,must fix later
				D3D12_RECT* d3dRect = g_RenderAllocator.Allocate<D3D12_RECT>(rectCount);
				for (size_t i = 0; i < rectCount; i++)
				{
					d3dRect[i].left = rects[i].left;
					d3dRect[i].right = rects[i].right();
					d3dRect[i].top = rects[i].top;
					d3dRect[i].bottom = rects[i].bottom();
				}
				commandList->ClearDepthStencilView(dsvHandle, clearFlags, depth, stencil, UINT(rectCount), d3dRect);
			}
			else
			{
				commandList->ClearDepthStencilView(dsvHandle, clearFlags, depth, stencil, 0, nullptr);
			}
		}

		void D3D12Renderer::ApplyRenderTargets(const IRenderTarget*const* renderTargets, std::size_t renderTargetCount, IDepthStencilBuffer* dsBuffer)
		{
			auto commandList = GetGraphicsCommandList();
			//TODO : actually should set pipeline description based on PipelineState rather than set them here
			auto rtvHandles = g_RenderAllocator.Allocate<D3D12_CPU_DESCRIPTOR_HANDLE>(renderTargetCount);
			for (std::size_t i = 0; i < renderTargetCount;++i)
			{
				rtvHandles[i] = static_cast<const D3D12RenderTarget*>(renderTargets[i])->GetCPUHandle();
				//TODO : Is it correct to use the first render target's sample description to set the pipeline desc?
			}
			auto dsHandle = static_cast<const D3D12DepthStencilBuffer*>(dsBuffer)->GetCPUHandle();
			commandList->OMSetRenderTargets(UINT(renderTargetCount), rtvHandles, FALSE, &dsHandle);
		}

		void D3D12Renderer::ApplyPipelineState(const PipelineState& state)
		{
			PipelineStateRootSignature stateAndSignature;
			auto hashValue = std::hash<PipelineState>{}(state);
			PipelineCacheMap::iterator it = mPipelineCache.end();
			bool createNewPSO{ true };
			{
				MutexLock lock(mtxPipelineCache);
				it = mPipelineCache.find(hashValue);
				if (it != mPipelineCache.end())
				{
					createNewPSO = false;
				}
			}

			if(createNewPSO)
			{
				stateAndSignature = CreateAndCachePipelineState(state, hashValue);
			}
			else
			{
				stateAndSignature = it->second;
			}

			auto commandList = GetGraphicsCommandList();
			commandList->SetPipelineState(stateAndSignature.pipelineState.Get());
			if (stateAndSignature.rootSignature)
			{
				commandList->SetGraphicsRootSignature(stateAndSignature.rootSignature.Get());
				BindShaderResources(state);
			}
		}

		void D3D12Renderer::ApplyViewports(const Viewport* viewports, std::size_t viewportCount)
		{
			auto commandList = GetGraphicsCommandList();
			auto D3D12Viewports = g_RenderAllocator.Allocate<D3D12_VIEWPORT>(viewportCount);
			for (std::uint8_t i = 0;i < viewportCount; ++i)
			{
				auto& viewport = D3D12Viewports[i];
				viewport.Width = viewports[i].width;
				viewport.Height = viewports[i].height;
				viewport.TopLeftX = viewports[i].left;
				viewport.TopLeftY = viewports[i].top;
				viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;
			}

			commandList->RSSetViewports(UINT(viewportCount), D3D12Viewports);
		}

		void D3D12Renderer::ApplyScissorRects(const ScissorRect* scissorRects, std::size_t scissorRectCount)
		{
			auto commandList = GetGraphicsCommandList();
			auto D3D12ScissorRects = g_RenderAllocator.Allocate<D3D12_RECT>(scissorRectCount);
			for (std::uint8_t i = 0;i < scissorRectCount; ++i)
			{
				auto& scissorRect = D3D12ScissorRects[i];
				scissorRect.left = scissorRects[i].left;
				scissorRect.top = scissorRects[i].top;
				scissorRect.right = scissorRects[i].left + scissorRects[i].width;
				scissorRect.bottom = scissorRects[i].top + scissorRects[i].height;
			}

			commandList->RSSetScissorRects(UINT(scissorRectCount), D3D12ScissorRects);
		}

		void D3D12Renderer::BindGPUBuffer(std::size_t slot, IGPUBuffer* pBuffer)
		{
			if (!pBuffer)
				return;
			auto bufferType = pBuffer->GetType();
			auto commandList = GetGraphicsCommandList();
			switch (bufferType)
			{
			case GPUBufferType::VERTEX:
			{
				D3D12_VERTEX_BUFFER_VIEW* bufferViews = g_RenderAllocator.Allocate<D3D12_VERTEX_BUFFER_VIEW>(1);
				bufferViews[0] = static_cast<D3D12VertexBuffer*>(pBuffer)->GetBufferView();
				commandList->IASetVertexBuffers(UINT(slot), 1, bufferViews);
				break;
			}
			case GPUBufferType::INDEX:
			{
				D3D12_INDEX_BUFFER_VIEW* bufferView = g_RenderAllocator.Allocate<D3D12_INDEX_BUFFER_VIEW>(1);
				bufferView[0] = static_cast<D3D12IndexBuffer*>(pBuffer)->GetBufferView();
				commandList->IASetIndexBuffer(bufferView);
				break;
			}
			default:
				break;
			}
		}

		void D3D12Renderer::Draw(const DrawParam& param)
		{
			auto commandList = GetGraphicsCommandList();
			if (param.drawType == DrawType::Vertex)
			{
				commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				commandList->DrawInstanced(UINT(param.vertexCount), UINT(param.instanceCount), UINT(param.firstVertex), UINT(param.baseInstance));
			}
			else
			{
				commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				commandList->DrawIndexedInstanced(UINT(param.indexCount), UINT(param.instanceCount), 
					UINT(param.firstIndex), UINT(param.baseIndex), UINT(param.baseInstance));
			}
		}

		void D3D12Renderer::ReportLiveObjects()const
		{
#ifndef NDEBUG
			if(mDXGIDebug) 
			{
				mDXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			}
#endif
		}

		D3D12Renderer::PipelineStateRootSignature D3D12Renderer::CreateAndCachePipelineState(const PipelineState& state, std::size_t hashValue)
		{
			PipelineStateRootSignature stateAndSignature;
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
			ApplyRasterizerState(state.rasterizerState, desc);
			ApplyBlendStates(0, state.blendStates, state.renderTargetCount, desc);
			ApplyDepthStencilState(state.depthStencilState, desc);
			Container::Vector<IShader*> shaders;
			ApplyShader(state.vs, desc);
			ApplyShader(state.fs, desc);
			ApplyShader(state.gs, desc);
			ApplyShader(state.hs, desc);
			ApplyShader(state.ds, desc);
			if (state.vs)
				shaders.push_back(state.vs);
			if (state.fs)
				shaders.push_back(state.fs);
			if (state.gs)
				shaders.push_back(state.gs);
			if (state.hs)
				shaders.push_back(state.hs);
			if (state.ds)
				shaders.push_back(state.ds);
			UpdatePSOInputLayout(state.inputLayouts, state.inputLayoutCount, desc);
			stateAndSignature.rootSignature = GetRootSignature(shaders);
			desc.pRootSignature = stateAndSignature.rootSignature.Get();
			desc.PrimitiveTopologyType = D3D12TypeMapper::MapPrimitiveType(state.primType);
			//TODO : should apply pipeline state based on PipelineState
			desc.NumRenderTargets = UINT(state.renderTargetCount);
			for (std::uint8_t i = 0; i < state.renderTargetCount; i++)
			{
				auto renderTexture = state.renderTargets[i]->GetTexture();
				if (i == 0)
				{
					desc.SampleDesc.Count = renderTexture->GetMultiSampleCount();
					desc.SampleDesc.Quality = renderTexture->GetMultiSampleQuality();
				}
				desc.RTVFormats[i] = D3D12TypeMapper::MapRenderFormat(renderTexture->GetRenderFormat());
			}
			desc.SampleMask = 0xfffffff;
			auto device = static_cast<D3D12Device*>(mDevice.get());
			stateAndSignature.pipelineState = device->CreateGraphicsPipelineState(&desc);
			if (!stateAndSignature.pipelineState)
			{
				LOG_ERROR("Failed to apply pipeline state!");
				return stateAndSignature;
			} 
			{
				MutexLock lock(mtxPipelineCache);
				//Must check again because other threads may create the same pipeline state object simultaneously
				auto it = mPipelineCache.find(hashValue);
				if (it == mPipelineCache.end())
					mPipelineCache.emplace(hashValue, stateAndSignature);
				else
					stateAndSignature = it->second;
			}
			return stateAndSignature;
		}

		ComPtr<ID3D12RootSignature> D3D12Renderer::GetRootSignature(const Container::Vector<IShader*>& shaders)
		{	
			//TODO : according to MSDN,pipeline state object can have 0 shader bound.But I found if I omit vertex shader
			//,the pipeline state creation will fail.Need to find the reason
			size_t seed = 0;
			boost::hash_combine(seed, shaders.size());
			for (const auto& s : shaders)
			{
				boost::hash_combine(seed, s->GetHash());
			}

			{
				MutexLock lock(mtxRootSignature);
				auto it = mRootSignatures.find(seed);
				if (it != mRootSignatures.end())
					return it->second;
			}

			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			Container::Vector<D3D12_ROOT_PARAMETER> cbParameters;
			D3D12_ROOT_SIGNATURE_FLAGS flags = 
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | 
				D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS | 
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
			for (std::size_t i = 0;i < shaders.size();++i)
			{
				switch (shaders[i]->GetType())
				{
				case ShaderType::VERTEX:
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::FRAGMENT:
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::GEOMETRY:
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::TESSELATION_CONTROL:		//hull
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::TESSELATION_EVALUATION:	//domain
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
					break;
				}
				const auto& parameters = static_cast<D3D12Shader*>(shaders[i])->GetRootParameters();
				cbParameters.insert(cbParameters.end(), parameters.begin(), parameters.end());
			}
			D3D12_ROOT_PARAMETER* pParameters = cbParameters.empty() ? nullptr : &cbParameters[0];
			rootSignatureDesc.Init(UINT(cbParameters.size()), pParameters, 0, nullptr, flags);

			ComPtr<ID3DBlob> signature;
			auto hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
			if (FAILED(hr))
			{
				return ComPtr<ID3D12RootSignature>();
			}

			ComPtr<ID3D12RootSignature> rootSignature;
			auto device = static_cast<D3D12Device*>(mDevice.get());
			rootSignature = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize());
			if (!rootSignature)
			{
				return rootSignature;
			}

			{
				MutexLock lock(mtxRootSignature);
				auto it = mRootSignatures.find(seed);
				if (it == mRootSignatures.end())
				{
					mRootSignatures[seed] = rootSignature;
				}
				else
				{
					rootSignature = it->second;
				}
			}
			return rootSignature;
		}

		void D3D12Renderer::BindShaderResources(const PipelineState& state)
		{
			std::size_t constantBuffers{ 0 };
			using ShaderResourceHandles = Container::UnorderedMap<ShaderType, Container::Vector<ShaderResourceHandle>>;
			static Foundation::ThreadLocalSingleton<ShaderResourceHandles> resourceHandleInstances;
			auto& resourceHandles = *resourceHandleInstances;
			resourceHandles.clear();
			if (state.vs)
			{
				constantBuffers += AnalyzeShaderRootResources(state.vs, resourceHandles);
			}
			if (state.fs)
			{
				constantBuffers += AnalyzeShaderRootResources(state.fs, resourceHandles);
			}
			if (state.gs)
			{
				constantBuffers += AnalyzeShaderRootResources(state.gs, resourceHandles);
			}
			if (state.hs)
			{
				constantBuffers += AnalyzeShaderRootResources(state.hs, resourceHandles);
			}
			if (state.ds)
			{
				constantBuffers += AnalyzeShaderRootResources(state.ds, resourceHandles);
			}
			Container::Vector<ID3D12DescriptorHeap*> descriptorHeaps;
			if (constantBuffers > 0)
			{
				auto constantHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
					true, UINT(constantBuffers), true);
				CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(constantHeap->cpuHandle);
				CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(constantHeap->gpuHandle);
				auto device = static_cast<D3D12Device*>(mDevice.get());
				for (auto it = resourceHandles.begin(); it != resourceHandles.end();++it)
				{
					for (auto& resourceHandle : it->second)
					{
						auto& resource = resourceHandle.resource;
						if (resource.type == D3D12RootResourceType::ConstantBuffers)
						{
							resourceHandle.handle = gpuHandle;
							for (auto i = 0;i < resource.bufferCount;++i)
							{
								const auto& cbuffer = resource.buffers[i];
								D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
								cbvDesc.BufferLocation = cbuffer.virtualAdress;
								cbvDesc.SizeInBytes = UINT(cbuffer.size);
								device->CreateConstantBufferView(&cbvDesc, cpuHandle);
								cpuHandle.Offset(constantHeap->incrementSize);
								gpuHandle.Offset(constantHeap->incrementSize);
							}
						}
					}
				}
				descriptorHeaps.push_back(D3D12DescriptorHeapManager::Instance()->GetHeap(constantHeap).Get());
			}
			auto commandList = GetGraphicsCommandList();
			if (!descriptorHeaps.empty())
			{
				commandList->SetDescriptorHeaps(UINT(descriptorHeaps.size()), &descriptorHeaps[0]);
			}
			UINT rootParameterIndex{ 0 };
			const static ShaderType shaderTypes[] = { ShaderType::VERTEX, ShaderType::FRAGMENT, ShaderType::GEOMETRY,
			ShaderType::TESSELATION_CONTROL, ShaderType::TESSELATION_EVALUATION };
			//Have to ensure iterate with the same order as root parameters.
			for (auto shaderType : shaderTypes)
			{
				auto it = resourceHandles.find(shaderType);
				if (it != resourceHandles.end())
				{
					auto& shaderResourceHandleList = it->second;
					for (const auto& shaderResourceHandle : shaderResourceHandleList)
					{
						const auto& resource = shaderResourceHandle.resource;
						const auto& handle = shaderResourceHandle.handle;
						if (resource.type == D3D12RootResourceType::ConstantBuffers)
						{
							commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
							++rootParameterIndex;
						}
					}
				}
			}
		}

		void D3D12Renderer::ApplyRasterizerState(const RasterizerState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		{
			D3D12_RASTERIZER_DESC* pDesc = &desc.RasterizerState;
			pDesc->FillMode = D3D12TypeMapper::MapFillMode(state.fillMode);
			pDesc->CullMode = D3D12TypeMapper::MapCullMode(state.cullMode);
			pDesc->ForcedSampleCount = 0;
			pDesc->FrontCounterClockwise = state.frontFace == WindingOrder::COUNTER_CLOCKWISE;
		}

		void D3D12Renderer::ApplyBlendStates(std::size_t firstRTIndex, const BlendState* states, std::size_t stateCount, 
			D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		{
			for (auto i = firstRTIndex; i < firstRTIndex + stateCount;++i)
			{
				//TODO these values should be set based on render unit status
				desc.BlendState.AlphaToCoverageEnable = FALSE;
				desc.BlendState.IndependentBlendEnable = FALSE;
				D3D12_RENDER_TARGET_BLEND_DESC* pDesc = &desc.BlendState.RenderTarget[i];
				//TODO logic enable is conflict with blend enable, can't be set to true both
				pDesc->LogicOpEnable = FALSE;
				pDesc->BlendEnable = states[i].enable;
				pDesc->BlendOp = D3D12TypeMapper::MapBlendOp(states[i].colorOp);
				pDesc->BlendOpAlpha = D3D12TypeMapper::MapBlendOp(states[i].alphaOp);
				pDesc->SrcBlend = D3D12TypeMapper::MapBlendFactor(states[i].srcColorFactor);
				pDesc->SrcBlendAlpha = D3D12TypeMapper::MapBlendFactor(states[i].srcAlphaFactor);
				pDesc->DestBlend = D3D12TypeMapper::MapBlendFactor(states[i].destColorFactor);
				pDesc->DestBlendAlpha = D3D12TypeMapper::MapBlendFactor(states[i].destAlphaFactor);
				//TODO RenderTargetWriteMask must be set to D3D12_COLOR_WRITE_ENABLE_ALL even if blend is disabled
				//I can't figure out the reason yet,need to delve into it
				pDesc->RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			}
		}

		void D3D12Renderer::ApplyDepthStencilState(const DepthStencilState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		{
			D3D12_DEPTH_STENCIL_DESC* pDesc = &desc.DepthStencilState;
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
			desc.DSVFormat = D3D12TypeMapper::MapRenderFormat(state.bufferFormat);
		}


		std::size_t D3D12Renderer::AnalyzeShaderRootResources(IShader *pShader, 
			Container::UnorderedMap<ShaderType, Container::Vector<D3D12Renderer::ShaderResourceHandle>>& resourceHandles)
		{
			std::size_t constantBuffers{ 0 };
			auto D3DShader = static_cast<D3D12Shader*>(pShader);
			auto boundResources = D3DShader->GetRootBoundResources();
			for (auto i = 0;i < D3DShader->GetRootParameterCount();++i)
			{
				resourceHandles[pShader->GetType()].emplace_back(boundResources[i]);
				if (boundResources[i].type == D3D12RootResourceType::ConstantBuffers)
					constantBuffers += boundResources[i].bufferCount;
			}
			return constantBuffers;
		}

		void D3D12Renderer::ApplyShader(IShader* pShader, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		{
			if (pShader)
			{
				auto d3d12shader = static_cast<D3D12Shader*>(pShader);
				auto byteCode = d3d12shader->GetByteCodeBuffer();
				auto byteCodeLength = d3d12shader->GetByteCodeBufferSize();
				switch (pShader->GetType())
				{
				case ShaderType::VERTEX:
					desc.VS.pShaderBytecode = byteCode;
					desc.VS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::FRAGMENT:
					desc.PS.pShaderBytecode = byteCode;
					desc.PS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::GEOMETRY:
					desc.GS.pShaderBytecode = byteCode;
					desc.GS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::TESSELATION_CONTROL:
					desc.HS.pShaderBytecode = byteCode;
					desc.HS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::TESSELATION_EVALUATION:
					desc.DS.pShaderBytecode = byteCode;
					desc.DS.BytecodeLength = byteCodeLength;
					break;
				default:
					break;
				}
			}
		}

		void D3D12Renderer::UpdatePSOInputLayout(const VertexInputLayout *inputLayouts, std::size_t layoutCount, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		{
			D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc = desc.InputLayout;
			if (layoutCount == 0)
			{
				inputLayoutDesc.NumElements = 0;
				inputLayoutDesc.pInputElementDescs = nullptr;
				return;
			}
			std::size_t inputElementCount{ 0 };
			for (std::size_t i = 0;i < layoutCount;++i)
			{
				inputElementCount += inputLayouts[i].componentCount;
			}
			auto pInputElementDesc = g_RenderAllocator.Allocate<D3D12_INPUT_ELEMENT_DESC>(inputElementCount);
			std::size_t i = 0;
			for (int j = 0;j < layoutCount;++j)
			{
				auto& inputLayout = inputLayouts[j];
				for (int k = 0;k < inputLayouts[j].componentCount;++k)
				{
					auto& component = inputLayouts[j].components[k];
					if(component.offset == 0)
						pInputElementDesc[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
					else
						pInputElementDesc[i].AlignedByteOffset = component.offset;
					pInputElementDesc[i].Format = D3D12TypeMapper::MapRenderFormat(component.format);
					pInputElementDesc[i].InputSlot = UINT(inputLayout.slot);
					pInputElementDesc[i].InputSlotClass = component.isInstance ? \
						D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
					pInputElementDesc[i].InstanceDataStepRate = component.isInstance ? component.instanceStepRate : 0;
					SemanticIndex semanticIndex;
					std::string semanticName;
					GetSemanticInfo(component.semantic, semanticIndex, semanticName);
					auto nameBufferSize = semanticName.length() + 1;
					auto nameArr = g_RenderAllocator.Allocate<char>(nameBufferSize);
					nameArr[nameBufferSize - 1] = 0;
					strncpy_s(nameArr, nameBufferSize, semanticName.c_str(), semanticName.length());
					pInputElementDesc[i].SemanticIndex = semanticIndex;
					pInputElementDesc[i].SemanticName = nameArr;
					++i;
				}
			}

			inputLayoutDesc.NumElements = UINT(inputElementCount);
			inputLayoutDesc.pInputElementDescs = pInputElementDesc;
		}





		SwapChain* D3D12Renderer::CreateSwapChain()
		{
			return new D3D12SwapChain(mDXGIFactory.Get(), GetCommandQueue(), mOutputWindow);
		}

		RenderPass* D3D12Renderer::CreateRenderPass(RenderPassType type)
		{
			if (type == RenderPassType::FORWARD)
				return new D3D12ForwardRenderPass;
			else
				return Renderer::CreateRenderPass(type);
		}

#ifndef NDEBUG
		void D3D12Renderer::InitDXGIDebug()
		{
			//HMODULE dxgiDebugHandle = ::GetModuleHandle("Dxgidebug.dll");
			//don't use GetModuleHandle because Dxgidebug.dll may not be loaded automatically by the app
			//so we just load the dll here explicitly
			HMODULE dxgiDebugHandle = ::LoadLibrary("Dxgidebug.dll");
			if (!dxgiDebugHandle)
			{
				LOG_WARNING("Can't get dxgidebug.dll module,errorCode:{0 : x}", ::GetLastError());
				return;
			}
			//the __stdcall declaration is required because windows APIs conform to stdcall convention
			//omit it will cause stack corruption
			using DXGIGetDebugInterfaceFunc = LRESULT (__stdcall *)(REFIID, void**);
			DXGIGetDebugInterfaceFunc pDXGIGetDebugInterface = reinterpret_cast<DXGIGetDebugInterfaceFunc>(::GetProcAddress(dxgiDebugHandle, "DXGIGetDebugInterface"));
			if (!pDXGIGetDebugInterface)
			{
				LOG_WARNING("Failed to get debug interface!");
				return;
			}
			pDXGIGetDebugInterface(IID_PPV_ARGS(&mDXGIDebug));
		}
#endif

		void D3D12Renderer::OnFrameBegin()
		{
			auto frameResourceIndex = GetFrameResourceIndex();
			mCmdEncoders[frameResourceIndex].for_each([](D3D12CommandEncoder& encoder) {
				encoder.Reset();
			});
			auto defaultRT = mSwapChain->GetCurrentRenderTarget();
			auto commandList = GetGraphicsCommandList();
			static_cast<D3D12RenderTarget*>(defaultRT)->TransitToPresentState(commandList);
			auto d3d12DSBuffer = static_cast<D3D12DepthStencilBuffer*>(
				mFrameResources[frameResourceIndex].defaultDepthStencilBuffer);
			d3d12DSBuffer->TransitToState(commandList, D3D12_RESOURCE_STATE_COMMON);
		}

		void D3D12Renderer::OnFrameUpdate()
		{

		}

		void D3D12Renderer::OnFrameEnd()
		{
			auto frameResourceIndex = GetFrameResourceIndex();
			auto defaultRenderTarget = mSwapChain->GetCurrentRenderTarget();
			auto renderTarget = static_cast<D3D12RenderTarget*>(defaultRenderTarget);
			Container::Vector<ID3D12CommandList*> commandLists;
			mCmdEncoders[frameResourceIndex].for_each([&commandLists](D3D12CommandEncoder& encoder) {
				commandLists.push_back(encoder.GetCommandList());
			});
			auto lastCmdList = static_cast<ID3D12GraphicsCommandList*>(commandLists.back());
			renderTarget->TransitToRTState(lastCmdList);
			renderTarget->TransitToPresentState(lastCmdList);

			D3D12StatefulResourceManager::Instance()->FixResourceStates(commandLists);

			for (auto cmdList : commandLists)
			{
				static_cast<ID3D12GraphicsCommandList*>(cmdList)->Close();
			}
			auto commandQueue = GetCommandQueue();
			commandQueue->ExecuteCommandLists(UINT(commandLists.size()), &commandLists[0]);
		}

		void D3D12Renderer::ResizeDepthStencilBuffer(IDepthStencilBuffer* depthStencilBuffer, 
			std::size_t width, std::size_t height)
		{
			auto D3DDepthStencilBuffer = dynamic_cast<D3D12DepthStencilBuffer*>(depthStencilBuffer);
			if (!D3DDepthStencilBuffer)
				return;

			D3DDepthStencilBuffer->Resize(width, height);
		}
	}
}