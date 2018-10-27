#include "d3d12renderer.h"
#include "d3d12swapchain.h"
#include "d3d12device.h"
#include "d3d12descriptorheapmanager.h"
#include "d3d12rendertarget.h"
#include "d3d12renderfence.h"
#include "d3d12constantbuffermanager.h"
#include "winwindow.h"
#include "configmanager.h"
#include "logger.h"
#include "common.h"
#include "framememoryallocator.h"
#include "d3d12statefulresourcemgr.h"
#include "d3d12vertexbuffer.h"
#include "d3d12indexbuffer.h"

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
		using WindowSystem::WinWindow;
		extern Foundation::FrameMemoryAllocator g_RenderAllocator;


		D3D12Renderer::~D3D12Renderer()
		{
			//Note:we should release resources in advance to make REPORT_LIVE_OBJECTS work correctly because if we let the share pointer
			//destructor run out of the scope,we cannot trace the objects 
			//device , swap chain and depth stencil buffer are parent class's members but we still release them here because we need to track alive resources
		}

		D3D12Renderer::D3D12Renderer(const SharedWindowPtr& pWindow, const SharedFileSystemPtr& fs) : Renderer(fs, pWindow)
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
			}
		}
#endif
		void D3D12Renderer::Start()
		{
			Renderer::Start();
#ifndef NDEBUG
			InitDXGIDebug();
#endif
			REPORT_LIVE_OBJECTS;
		}

		void D3D12Renderer::ShutDown()
		{
			LOG_INFO("Start to clean up render resources.");
			Renderer::ShutDown();
			D3D12RenderTargetManager::Instance()->Clear();
			D3D12DescriptorHeapManager::Instance()->Clear();
			D3D12ConstantBufferManager::Instance()->Clear();
			D3D12StatefulResourceMgr::Instance()->Clear();
			mCommandQueue.Reset();
			mDXGIFactory.Reset();
			mPipelineCache.clear();
			mRootSignatures.clear();
			for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				mFrontCmdEncoder[i].Clear();
				mBackCmdEncoder[i].Clear();
				mCmdEncoders[i].for_each([](D3D12CommandEncoder& encoder) {
					encoder.Clear();
				});
			}
			REPORT_LIVE_OBJECTS;
			LOG_INFO("Finished reporting live objects!");
		}

		ID3D12CommandQueue* D3D12Renderer::GetCommandQueue()
		{
			return mCommandQueue.Get();
		}

		ID3D12GraphicsCommandList* D3D12Renderer::GetGraphicsCommandList()
		{
			return mCmdEncoders[mFrameResourceIndex]->GetCommandList();
		}

		IRenderFence* D3D12Renderer::CreateRenderFence()
		{
			return new D3D12RenderFence(static_cast<D3D12Device*>(mDevice.get()), 0);
		}

		IDevice* D3D12Renderer::CreateDevice()
		{
			auto device = new D3D12Device(mDXGIFactory.Get(), mFs);
			D3D12_COMMAND_QUEUE_DESC desc = {};
			mCommandQueue = device->CreateCommandQueue(&desc);
			return device;
		}

		void D3D12Renderer::ClearRenderTarget(const SharedRenderTargetPtr& rt, const ColorF& color, const RectIList* rects)
		{
			D3D12RenderTarget *pTarget = static_cast<D3D12RenderTarget*>(rt.get());
			assert(pTarget);
			//should check the type of the rt to transit it from previous state to render target state
			//currently just check back buffer render target
			if (rt->IsSwapChainRenderTarget())
			{
				pTarget->TransitToRTState(mFrontCmdEncoder[mFrameResourceIndex].GetCommandList());
			}

			//cache render target to prevent it from being released before GPU execute ClearRenderTargetView

			auto commandList = GetGraphicsCommandList();
			const float clearColor[] = { color.r, color.g, color.b, color.a };
			auto rtvHandle = pTarget->GetCPUHandle();
			if (rects && !rects->empty())
			{
				//TODO : This implementation is wrong.Should allocate frame memory not local memory,must fix later
				D3D12_RECT* d3dRect = g_RenderAllocator.Allocate<D3D12_RECT>(rects->size());
				for (size_t i = 0; i < rects->size(); i++)
				{
					d3dRect[i].left = (*rects)[i].left;
					d3dRect[i].right = (*rects)[i].right();
					d3dRect[i].top = (*rects)[i].top;
					d3dRect[i].bottom = (*rects)[i].bottom();
				}
				commandList->ClearRenderTargetView(rtvHandle, clearColor, rects->size(), d3dRect);
			}
			else
			{
				commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
			}
		}

		void D3D12Renderer::ClearDepthStencilBuffer(const SharedDepthStencilBufferPtr& buffer, DepthStencilClearFlags flags, 
			float depth, std::uint8_t stencil, const RectIList* rects)
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
			auto dsvHandle = static_cast<D3D12DepthStencilBuffer*>(buffer.get())->GetCPUHandle();
			auto commandList = GetGraphicsCommandList();
			if (rects && !rects->empty())
			{
				//TODO : This implementation is wrong.Should allocate frame memory not local memory,must fix later
				D3D12_RECT* d3dRect = g_RenderAllocator.Allocate<D3D12_RECT>(rects->size());
				for (size_t i = 0; i < rects->size(); i++)
				{
					d3dRect[i].left = (*rects)[i].left;
					d3dRect[i].right = (*rects)[i].right();
					d3dRect[i].top = (*rects)[i].top;
					d3dRect[i].bottom = (*rects)[i].bottom();
				}
				commandList->ClearDepthStencilView(dsvHandle, clearFlags, depth, stencil, rects->size(), d3dRect);
			}
			else
			{
				commandList->ClearDepthStencilView(dsvHandle, clearFlags, depth, stencil, 0, nullptr);
			}
		}

		void D3D12Renderer::ApplyRenderTargets(const container::vector<SharedRenderTargetPtr>& renderTargets, const SharedDepthStencilBufferPtr& dsBuffer)
		{
			assert(renderTargets.size() <= MAX_RENDER_TARGET_COUNT);
			auto commandList = GetGraphicsCommandList();
			auto renderTargetCount = renderTargets.size();
			//TODO : actually should set pipeline description based on PipelineState rather than set them here
			auto rtvHandles = g_RenderAllocator.Allocate<D3D12_CPU_DESCRIPTOR_HANDLE>(renderTargetCount);
			for (std::size_t i = 0; i < renderTargetCount;++i)
			{
				rtvHandles[i] = static_cast<const D3D12RenderTarget*>(renderTargets[i].get())->GetCPUHandle();
				//TODO : Is it correct to use the first render target's sample description to set the pipeline desc?
			}
			auto dsHandle = static_cast<const D3D12DepthStencilBuffer*>(dsBuffer.get())->GetCPUHandle();
			commandList->OMSetRenderTargets(renderTargetCount, rtvHandles, FALSE, &dsHandle);
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

		void D3D12Renderer::BindGPUBuffer(std::uint8_t slot, const SharedGPUBufferPtr& pBuffer)
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
				bufferViews[0] = static_cast<D3D12VertexBuffer*>(pBuffer.get())->GetBufferView();
				commandList->IASetVertexBuffers(slot, 1, bufferViews);
				break;
			}
			case GPUBufferType::INDEX:
			{
				D3D12_INDEX_BUFFER_VIEW* bufferView = g_RenderAllocator.Allocate<D3D12_INDEX_BUFFER_VIEW>(1);
				bufferView[0] = static_cast<D3D12IndexBuffer*>(pBuffer.get())->GetBufferView();
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
				commandList->DrawInstanced(param.vertexCount, param.instanceCount, param.firstVertex, param.baseInstance);
			}
			else
			{
				D3D12_VIEWPORT	vp{};
				vp.Width = 800;
				vp.Height = 600;
				vp.TopLeftX = 0;
				vp.TopLeftY = 0;
				vp.MinDepth = 0.0f;
				vp.MaxDepth = 1.0f;

				D3D12_RECT scissorRect;
				scissorRect.left = scissorRect.top = 0;
				scissorRect.right = 800;
				scissorRect.bottom = 600;
				commandList->RSSetScissorRects(1, &scissorRect);
				commandList->RSSetViewports(1, &vp);
				commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				commandList->DrawIndexedInstanced(param.indexCount, param.instanceCount, param.firstIndex, param.baseIndex, param.baseInstance);
			}
		}

		D3D12Renderer::PipelineStateRootSignature D3D12Renderer::CreateAndCachePipelineState(const PipelineState& state, std::size_t hashValue)
		{
			PipelineStateRootSignature stateAndSignature;
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
			ApplyRasterizerState(state.rasterizerState, desc);
			ApplyBlendStates(0, state.blendStates, state.renderTargetCount, desc);
			ApplyDepthStencilState(state.depthStencilState, desc);
			container::vector<IShader*> shaders;
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
			desc.NumRenderTargets = state.renderTargetCount;
			for (size_t i = 0; i < sizeof(desc.RTVFormats) / sizeof(DXGI_FORMAT); i++)
			{
				if (i == 0)
				{
					desc.SampleDesc.Count = state.renderTargets[i]->GetSampleCount();
					desc.SampleDesc.Quality = state.renderTargets[i]->GetSampleQuality();
				}
				if (i < state.renderTargetCount)
				{
					desc.RTVFormats[i] = D3D12TypeMapper::MapRenderFormat(state.renderTargets[i]->GetRenderFormat());
				}
				else
				{
					desc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
				}
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

		ComPtr<ID3D12RootSignature> D3D12Renderer::GetRootSignature(const container::vector<IShader*>& shaders)
		{	
			//TODO : according to MSDN,pipeline state object can have 0 shader bound.But I found if I omit vertex shader
			//,the pipeline state creation will fail.Need to find the reason
			size_t seed = 0;
			boost::hash_combine(seed, shaders.size());
			for (const auto& s : shaders)
			{
				boost::hash_combine(seed, s->GetHashValue());
			}

			{
				MutexLock lock(mtxRootSignature);
				auto it = mRootSignatures.find(seed);
				if (it != mRootSignatures.end())
					return it->second;
			}

			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			container::vector<D3D12_ROOT_PARAMETER> cbParameters;
			for (std::size_t i = 0;i < shaders.size();++i)
			{
				const auto& parameters = static_cast<D3D12Shader*>(shaders[i])->GetRootParameters();
				cbParameters.insert(cbParameters.end(), parameters.begin(), parameters.end());
			}
			D3D12_ROOT_PARAMETER* pParameters = cbParameters.empty() ? nullptr : &cbParameters[0];
			rootSignatureDesc.Init(cbParameters.size(), pParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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
					rootSignature = it->second;
			}
			return rootSignature;
		}

		void D3D12Renderer::BindShaderResources(const PipelineState& state)
		{
			std::size_t constantBuffers{ 0 };
			container::unordered_map<ShaderType, container::vector<ShaderResourceHandle>> boundResources;
			if (state.vs)
			{
				constantBuffers += AnalyzeShaderRootResources(state.vs, boundResources);
			}
			if (state.fs)
			{
				constantBuffers += AnalyzeShaderRootResources(state.fs, boundResources);
			}
			if (state.gs)
			{
				constantBuffers += AnalyzeShaderRootResources(state.gs, boundResources);
			}
			if (state.hs)
			{
				constantBuffers += AnalyzeShaderRootResources(state.hs, boundResources);
			}
			if (state.ds)
			{
				constantBuffers += AnalyzeShaderRootResources(state.ds, boundResources);
			}
			container::vector<ID3D12DescriptorHeap*> descriptorHeaps;
			if (constantBuffers > 0)
			{
				auto constantHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
					true, constantBuffers, true);
				CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(constantHeap->cpuHandle);
				CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(constantHeap->gpuHandle);
				auto device = static_cast<D3D12Device*>(mDevice.get());
				for (auto it = boundResources.begin(); it != boundResources.end();++it)
				{
					for (auto& resourceHandle : it->second)
					{
						auto& resource = resourceHandle.resource;
						if (resource.type == D3D12RootResourceType::ConstantBuffers)
						{
							resourceHandle.handle = gpuHandle;
							for (auto& cbuffer : resource.buffers)
							{
								D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
								cbvDesc.BufferLocation = cbuffer.virtualAdress;
								cbvDesc.SizeInBytes = cbuffer.size;
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
				commandList->SetDescriptorHeaps(descriptorHeaps.size(), &descriptorHeaps[0]);
			}
			UINT rootParameterIndex{ 0 };
			for (const auto& pair : boundResources)
			{
				for (std::size_t i = 0;i < pair.second.size();++i)
				{
					const auto& resource = pair.second[i].resource;
					const auto& handle = pair.second[i].handle;
					if (resource.type == D3D12RootResourceType::ConstantBuffers)
					{
						commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
						++rootParameterIndex;
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

		void D3D12Renderer::ApplyBlendStates(const std::uint8_t firstRTIndex, const BlendState* states, const std::uint8_t stateCount, 
			D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		{
			for (int i = firstRTIndex; i < firstRTIndex + stateCount;++i)
			{
				//TODO these values should be set based on render node status
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
			container::unordered_map<ShaderType, container::vector<D3D12Renderer::ShaderResourceHandle>>& resourceHandles)
		{
			std::size_t constantBuffers{ 0 };
			for (const auto& resource : static_cast<D3D12Shader*>(pShader)->GetRootBoundResources())
			{
				resourceHandles[pShader->GetType()].emplace_back(resource);
				if (resource.type == D3D12RootResourceType::ConstantBuffers)
					constantBuffers += resource.buffers.size();
			}
			return constantBuffers;
		}

		void D3D12Renderer::ApplyViewports(const RectFList& vp, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		{

		}

		void D3D12Renderer::ApplyScissorRects(const RectFList& scissorRects, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		{

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

		void D3D12Renderer::UpdatePSOInputLayout(const VertexInputLayout *inputLayouts, std::uint8_t  layoutCount, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
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
					pInputElementDesc[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
					pInputElementDesc[i].Format = D3D12TypeMapper::MapRenderFormat(component.format);
					pInputElementDesc[i].InputSlot = inputLayout.slot;
					pInputElementDesc[i].InputSlotClass = component.isInstance ? \
						D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
					pInputElementDesc[i].InstanceDataStepRate = component.isInstance ? component.instanceStepRate : 0;
					pInputElementDesc[i].SemanticIndex = component.semanticIndex;
					pInputElementDesc[i].SemanticName = component.semanticItem.name;
					++i;
				}
			}

			inputLayoutDesc.NumElements = inputElementCount;
			inputLayoutDesc.pInputElementDescs = pInputElementDesc;
		}





		ISwapChain* D3D12Renderer::CreateSwapChain()
		{
			return new D3D12SwapChain(mDXGIFactory.Get(), GetCommandQueue(), mOutputWindow.get());
		}

		IDepthStencilBuffer* D3D12Renderer::CreateDepthStencilBuffer(std::size_t width, std::size_t height)
		{
			return new D3D12DepthStencilBuffer(width, height);
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

		void D3D12Renderer::BeginFrame()
		{
			Renderer::BeginFrame();
			D3D12DescriptorHeapManager::Instance()->EraseTransientAllocation(mFrameResourceIndex);
			D3D12ConstantBufferManager::Instance()->ResetBuffers(mFrameResourceIndex);
			mCmdEncoders[mFrameResourceIndex].for_each([](D3D12CommandEncoder& encoder) {
				encoder.Reset();
			});
			mFrontCmdEncoder[mFrameResourceIndex].Reset();
			mBackCmdEncoder[mFrameResourceIndex].Reset();
		}

		void D3D12Renderer::DoFrame()
		{
			Renderer::DoFrame();
		}

		void D3D12Renderer::EndFrame()
		{
			auto defaultRenderTarget = mSwapChain->GetDefaultRenderTarget();
			auto renderTarget = static_cast<D3D12RenderTarget*>(defaultRenderTarget.get());
			container::vector<ID3D12CommandList*> commandLists{
				mFrontCmdEncoder[mFrameResourceIndex].GetCommandList(),
			};
			mCmdEncoders[mFrameResourceIndex].for_each([&commandLists](D3D12CommandEncoder& encoder) {
				commandLists.push_back(encoder.GetCommandList());
			});
			commandLists.push_back(mBackCmdEncoder[mFrameResourceIndex].GetCommandList());

			D3D12RenderTargetManager::Instance()->Synchronize();
			renderTarget->TransitToPresentState(mBackCmdEncoder[mFrameResourceIndex].GetCommandList());
			D3D12StatefulResourceMgr::Instance()->FixResourceStates(commandLists);

			for (auto cmdList : commandLists)
			{
				static_cast<ID3D12GraphicsCommandList*>(cmdList)->Close();
			}
			auto commandQueue = GetCommandQueue();
			commandQueue->ExecuteCommandLists(commandLists.size(), &commandLists[0]);
			Renderer::EndFrame();
		}



	}
}