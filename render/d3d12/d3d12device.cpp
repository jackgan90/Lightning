#include <d3dx12.h>
#include <d3dcompiler.h>
#include <functional>
//#include "rendererfactory.h"
#include "common.h"
#include "irenderer.h"
#include "d3d12device.h"
#include "d3d12rendertarget.h"
#include "d3d12depthstencilbuffer.h"
#include "d3d12shader.h"
#include "d3d12typemapper.h"
#include "d3d12vertexbuffer.h"
#include "d3d12indexbuffer.h"
#include "renderconstants.h"
#include "semantics.h"
#include "shadermanager.h"
#include "logger.h"
#include "configmanager.h"
#include "ringallocator.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		using Foundation::RingAllocator;

		extern RingAllocator g_RenderAllocator;
		const char* const DEFAULT_VS_SOURCE =
			"cbuffer VS_IN : register(b0)\n"
			"{\n"
			"	float4x4 wvp;\n"
			"};\n"
			"float4 main(float3 position:POSITION):SV_POSITION\n"
			"{\n"
				"return mul(wvp, float4(position, 1.0f));\n"
			"}\n";
		const char* const DEFAULT_PS_SOURCE =
			"float4 main(void):SV_TARGET\n"
			"{\n"
				"return float4(1.0f, 0.0f, 0.0f, 1.0f);\n"
			"}\n";
		D3D12Device::D3D12Device(IDXGIFactory4* factory, const SharedFileSystemPtr& fs)
			:Device(), mFs(fs), mPipelineDesc{}
		{
			CreateNativeDevice(factory);
			D3D12_COMMAND_QUEUE_DESC desc = {};
			HRESULT hr = mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&mCommandQueue));
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create command queue!");
			}
			for (size_t i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				hr = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mFrameResources[i].commandAllocator));
				if (FAILED(hr))
				{
					throw DeviceInitException("Failed to create command allocator!");
				}
			}
			hr = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mFrameResources[0].commandAllocator.Get(), nullptr, IID_PPV_ARGS(&mCommandList));
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create command list!");
			}
			mShaderMgr = std::make_unique<D3D12ShaderManager>(this, fs);
			mPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			//should create first default pipeline state
			SetUpDefaultPipelineStates();
			mCommandList->Close();
		}

		D3D12Device::~D3D12Device()
		{
			mShaderMgr.reset();
		}

		void D3D12Device::CreateNativeDevice(IDXGIFactory4* factory)
		{
			ComPtr<IDXGIAdapter1> adaptor;
			int adaptorIndex = 0;
			bool adaptorFound = false;
			HRESULT hr;

			while (factory->EnumAdapters1(adaptorIndex, &adaptor) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC1 desc;
				adaptor->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					adaptorIndex++;
					continue;
				}
				hr = D3D12CreateDevice(adaptor.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
				if (SUCCEEDED(hr))
				{
					adaptorFound = true;
					break;
				}
				adaptorIndex++;
			}
			if (!adaptorFound)
			{
				throw DeviceInitException("Can't find hardware d3d12 adaptor!");
			}
			hr = D3D12CreateDevice(adaptor.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice));
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create d3d12 device!");
			}
		}


		void D3D12Device::ClearRenderTarget(const SharedRenderTargetPtr& rt, const ColorF& color, const RectIList* rects)
		{
			D3D12RenderTarget *pTarget = static_cast<D3D12RenderTarget*>(rt.get());
			assert(pTarget);
			ComPtr<ID3D12Resource> nativeRenderTarget = pTarget->GetNative();
			//should check the type of the rt to transit it from previous state to render target state
			//currently just check back buffer render target
			if (rt->IsSwapChainRenderTarget())
			{
				mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRenderTarget.Get(),
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
			}

			//cache render target to prevent it from being released before GPU execute ClearRenderTargetView
			CacheResourceReference(rt);

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
				mCommandList->ClearRenderTargetView(rtvHandle, clearColor, rects->size(), d3dRect);
			}
			else
			{
				mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
			}
		}

		void D3D12Device::ClearDepthStencilBuffer(const SharedDepthStencilBufferPtr& buffer, DepthStencilClearFlags flags, 
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
			CacheResourceReference(buffer);
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
				mCommandList->ClearDepthStencilView(dsvHandle, clearFlags, depth, stencil, rects->size(), d3dRect);
			}
			else
			{
				mCommandList->ClearDepthStencilView(dsvHandle, clearFlags, depth, stencil, 0, nullptr);
			}
		}



		SharedVertexBufferPtr D3D12Device::CreateVertexBuffer(std::uint32_t bufferSize, const std::vector<VertexComponent>& components)
		{
			const VertexComponent *comps{ nullptr };
			if (!components.empty())
			{
				comps = &components[0];
			}
			return std::make_shared<D3D12VertexBuffer>(mDevice.Get(), bufferSize, comps, components.size());
		}

		SharedIndexBufferPtr D3D12Device::CreateIndexBuffer(std::uint32_t bufferSize, IndexType type)
		{
			return std::make_shared<D3D12IndexBuffer>(mDevice.Get(), bufferSize, type);
		}

		void D3D12Device::ApplyRasterizerState(const RasterizerState& state)
		{
			Device::ApplyRasterizerState(state);
			D3D12_RASTERIZER_DESC* pDesc = &mPipelineDesc.RasterizerState;
			pDesc->FillMode = D3D12TypeMapper::MapFillMode(state.fillMode);
			pDesc->CullMode = D3D12TypeMapper::MapCullMode(state.cullMode);
			pDesc->FrontCounterClockwise = state.frontFace == WindingOrder::COUNTER_CLOCKWISE;
		}

		void D3D12Device::ApplyBlendStates(const std::uint8_t firstRTIndex, const BlendState* states, const std::uint8_t stateCount)
		{
			Device::ApplyBlendStates(firstRTIndex, states, stateCount);
			for (std::size_t i = firstRTIndex; i < firstRTIndex + stateCount;++i)
			{
				D3D12_RENDER_TARGET_BLEND_DESC* pDesc = &mPipelineDesc.BlendState.RenderTarget[i];
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

		void D3D12Device::ApplyDepthStencilState(const DepthStencilState& state)
		{
			Device::ApplyDepthStencilState(state);
			D3D12_DEPTH_STENCIL_DESC* pDesc = &mPipelineDesc.DepthStencilState;
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
			mPipelineDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
			if (state.depthTestEnable || state.stencilEnable)
			{
				if (mCurrentDSBuffer)
				{
					mPipelineDesc.DSVFormat = D3D12TypeMapper::MapRenderFormat(mCurrentDSBuffer->GetRenderFormat());
				}
			}

		}

		void D3D12Device::ApplyPipelineState(const PipelineState& state)
		{
			Device::ApplyPipelineState(state);
			ComPtr<ID3D12PipelineState> pipelineState;
			auto hashValue = std::hash<PipelineState>{}(state);
			auto cachedStateObject = mPipelineCache.find(hashValue);
			if (cachedStateObject != mPipelineCache.end())
			{
				pipelineState = cachedStateObject->second;
			}
			else
			{
				pipelineState = CreateAndCachePipelineState(state, hashValue);
			}
			mD3D12PipelineState = pipelineState;
			mCommandList->SetPipelineState(mD3D12PipelineState.Get());
			if (mPipelineDesc.pRootSignature)
			{
				mCommandList->SetGraphicsRootSignature(mPipelineDesc.pRootSignature);
				auto& frameResource = mFrameResources[mFrameResourceIndex];
				ExtractShaderDescriptorHeaps();
				//TODO : cancel all heap binding when empty?
				if (!frameResource.descriptorHeaps.empty())
				{
					mCommandList->SetDescriptorHeaps(frameResource.descriptorHeaps.size(), &frameResource.descriptorHeaps[0]);
				}
				BindShaderResources();
			}
		}

		void D3D12Device::ExtractShaderDescriptorHeaps()
		{
			ExtractShaderDescriptorHeaps(mDevicePipelineState.vs);
			ExtractShaderDescriptorHeaps(mDevicePipelineState.fs);
			ExtractShaderDescriptorHeaps(mDevicePipelineState.gs);
			ExtractShaderDescriptorHeaps(mDevicePipelineState.hs);
			ExtractShaderDescriptorHeaps(mDevicePipelineState.ds);
		}

		void D3D12Device::ExtractShaderDescriptorHeaps(IShader* pShader)
		{
			if (pShader)
			{
				auto pD3D12Shader = static_cast<D3D12Shader*>(pShader);
				auto const& boundResources = pD3D12Shader->GetRootBoundResources();
				auto& frameResource = mFrameResources[mFrameResourceIndex];
				for (std::size_t i = 0; i < boundResources.size(); ++i)
				{
					const auto& boundResource = boundResources[i];
					if (boundResource.type == D3D12RootBoundResourceType::DescriptorTable)
					{
						frameResource.descriptorHeaps.push_back(boundResource.descriptorTableHeap.Get());
					}
				}
			}
		}

		void D3D12Device::BindShaderResources()
		{
			std::size_t rootParameterIndex{ 0 };
			if (mDevicePipelineState.vs)
			{
				BindShaderResources(mDevicePipelineState.vs, rootParameterIndex);
				rootParameterIndex += static_cast<D3D12Shader*>(mDevicePipelineState.vs)->GetRootParameterCount();
			}
			if (mDevicePipelineState.fs)
			{
				BindShaderResources(mDevicePipelineState.fs, rootParameterIndex);
				rootParameterIndex += static_cast<D3D12Shader*>(mDevicePipelineState.fs)->GetRootParameterCount();
			}
			if (mDevicePipelineState.gs)
			{
				BindShaderResources(mDevicePipelineState.gs, rootParameterIndex);
				rootParameterIndex += static_cast<D3D12Shader*>(mDevicePipelineState.gs)->GetRootParameterCount();
			}
			if (mDevicePipelineState.hs)
			{
				BindShaderResources(mDevicePipelineState.hs, rootParameterIndex);
				rootParameterIndex += static_cast<D3D12Shader*>(mDevicePipelineState.hs)->GetRootParameterCount();
			}
			if (mDevicePipelineState.ds)
			{
				BindShaderResources(mDevicePipelineState.ds, rootParameterIndex);
				rootParameterIndex += static_cast<D3D12Shader*>(mDevicePipelineState.ds)->GetRootParameterCount();
			}
		}

		void D3D12Device::BindShaderResources(IShader* pShader, UINT rootParameterIndex)
		{
			auto pD3D12Shader = static_cast<D3D12Shader*>(pShader);
			auto const& boundResources = pD3D12Shader->GetRootBoundResources();
			for (std::size_t i = 0;i < boundResources.size();++i)
			{
				const auto& boundResource = boundResources[i];
				switch (boundResource.type)
				{
				case D3D12RootBoundResourceType::DescriptorTable:
					mCommandList->SetGraphicsRootDescriptorTable(rootParameterIndex + i, boundResource.descriptorTableHandle);
					break;
				case D3D12RootBoundResourceType::ConstantBufferView:
					mCommandList->SetGraphicsRootConstantBufferView(rootParameterIndex + i, boundResource.GPUVirtualAddress);
					break;
				case D3D12RootBoundResourceType::ShaderResourceView:
					mCommandList->SetGraphicsRootShaderResourceView(rootParameterIndex + i, boundResource.GPUVirtualAddress);
					break;
				case D3D12RootBoundResourceType::UnorderedAccessView:
					mCommandList->SetGraphicsRootUnorderedAccessView(rootParameterIndex + i, boundResource.GPUVirtualAddress);
					break;
				case D3D12RootBoundResourceType::Constant:
					mCommandList->SetGraphicsRoot32BitConstants(rootParameterIndex + i, boundResource.constant32BitValue.num32BitValues, boundResource.constant32BitValue.p32BitValues, boundResource.constant32BitValue.dest32BitValueOffset);
					break;
				default:
					break;
				}
			}
		}

		void D3D12Device::ApplyViewports(const RectFList& vp)
		{

		}

		void D3D12Device::ApplyScissorRects(const RectFList& scissorRects)
		{

		}

		ComPtr<ID3D12PipelineState> D3D12Device::CreateAndCachePipelineState(const PipelineState& state, std::size_t hashValue)
		{
			ComPtr<ID3D12PipelineState> pipelineState;
			ApplyRasterizerState(state.rasterizerState);
			ApplyBlendStates(0, state.blendStates, state.outputRenderTargetCount);
			ApplyDepthStencilState(state.depthStencilState);
			std::vector<IShader*> shaders;
			ApplyShader(state.vs);
			ApplyShader(state.fs);
			ApplyShader(state.gs);
			ApplyShader(state.hs);
			ApplyShader(state.ds);
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
			UpdatePSOInputLayout(state.inputLayouts, state.inputLayoutCount);
			auto rootSignature = GetRootSignature(shaders);
			mPipelineDesc.pRootSignature = rootSignature.Get();
			mPipelineDesc.PrimitiveTopologyType = D3D12TypeMapper::MapPrimitiveType(state.primType);
			//TODO : should apply pipeline state based on PipelineState
			mPipelineDesc.NumRenderTargets = mDevicePipelineState.outputRenderTargetCount;
			for (size_t i = 0; i < sizeof(mPipelineDesc.RTVFormats) / sizeof(DXGI_FORMAT); i++)
			{
				if (i < mDevicePipelineState.outputRenderTargetCount)
				{
					mPipelineDesc.RTVFormats[i] = D3D12TypeMapper::MapRenderFormat(mDevicePipelineState.renderTargets[i]->GetRenderFormat());
				}
				else
				{
					mPipelineDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
				}
			}
			mPipelineDesc.SampleDesc.Count = 1;
			mPipelineDesc.SampleDesc.Quality = 0;
			mPipelineDesc.SampleMask = 0xfffffff;
			auto hr = mDevice->CreateGraphicsPipelineState(&mPipelineDesc, IID_PPV_ARGS(&pipelineState));
			if (FAILED(hr))
			{
				LOG_ERROR("Failed to apply pipeline state!");
				return pipelineState;
			} 
			mPipelineCache.emplace(hashValue, pipelineState);
			return pipelineState;
		}

		void D3D12Device::SetUpDefaultPipelineStates()
		{
			mDefaultShaders[ShaderType::VERTEX] = mShaderMgr->CreateShaderFromSource(ShaderType::VERTEX, "[Built-in]default.vs", DEFAULT_VS_SOURCE, ShaderDefine());
			mDefaultShaders[ShaderType::FRAGMENT] = mShaderMgr->CreateShaderFromSource(ShaderType::FRAGMENT, "[Built-in]default.ps", DEFAULT_PS_SOURCE, ShaderDefine()); 
			mDevicePipelineState.vs = mDefaultShaders[ShaderType::VERTEX].get();
			VertexComponent defaultComponent;
			VertexInputLayout layout;
			layout.slot = 0;
			layout.components = &defaultComponent;
			layout.componentCount = 1;
			mDevicePipelineState.inputLayouts = &layout;
			mDevicePipelineState.inputLayoutCount = 1;
			mDevicePipelineState.depthStencilState.depthTestEnable = false;
			mDevicePipelineState.primType = PrimitiveType::TRIANGLE_LIST;
			mDevicePipelineState.outputRenderTargetCount = 0;
			ApplyPipelineState(mDevicePipelineState);
		}

		SharedShaderPtr D3D12Device::CreateShader(ShaderType type, const std::string& shaderName, 
			const char* const shaderSource, const ShaderDefine& defineMap)
		{
			return std::make_shared<D3D12Shader>(mDevice.Get(), type, shaderName, DEFAULT_SHADER_ENTRY, shaderSource);
		}

		void D3D12Device::ApplyShader(IShader* pShader)
		{
			if (pShader)
			{
				auto d3d12shader = static_cast<D3D12Shader*>(pShader);
				auto byteCode = d3d12shader->GetByteCodeBuffer();
				auto byteCodeLength = d3d12shader->GetByteCodeBufferSize();
				switch (pShader->GetType())
				{
				case ShaderType::VERTEX:
					mPipelineDesc.VS.pShaderBytecode = byteCode;
					mPipelineDesc.VS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::FRAGMENT:
					mPipelineDesc.PS.pShaderBytecode = byteCode;
					mPipelineDesc.PS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::GEOMETRY:
					mPipelineDesc.GS.pShaderBytecode = byteCode;
					mPipelineDesc.GS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::TESSELATION_CONTROL:
					mPipelineDesc.HS.pShaderBytecode = byteCode;
					mPipelineDesc.HS.BytecodeLength = byteCodeLength;
					break;
				case ShaderType::TESSELATION_EVALUATION:
					mPipelineDesc.DS.pShaderBytecode = byteCode;
					mPipelineDesc.DS.BytecodeLength = byteCodeLength;
					break;
				default:
					break;
				}
			}
		}

		void D3D12Device::UpdatePSOInputLayout(const VertexInputLayout *inputLayouts, std::uint8_t  layoutCount)
		{
			D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc = mPipelineDesc.InputLayout;
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

		ComPtr<ID3D12RootSignature> D3D12Device::GetRootSignature(const std::vector<IShader*>& shaders)
		{	
			//TODO : according to MSDN,pipeline state object can have 0 shader bound.But I found if I omit vertex shader
			//,the pipeline state creation will fail.Need to find the reason
			size_t seed = 0;
			boost::hash_combine(seed, shaders.size());
			for (const auto& s : shaders)
			{
				boost::hash_combine(seed, s->GetHashValue());
			}
			auto it = mRootSignatures.find(seed);
			if (it != mRootSignatures.end())
				return it->second;

			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			std::vector<D3D12_ROOT_PARAMETER> cbParameters;
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
			hr = mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
			if (FAILED(hr))
			{
				return ComPtr<ID3D12RootSignature>();
			}
			mRootSignatures[seed] = rootSignature;
			return rootSignature;
		}

		void D3D12Device::BeginFrame(const std::size_t frameResourceIndex)
		{
			Device::BeginFrame(frameResourceIndex);
			mFrameResources[frameResourceIndex].Release(true);
			mCommandList->Reset(mFrameResources[frameResourceIndex].commandAllocator.Get(), nullptr);
		}

		void D3D12Device::ApplyRenderTargets(const SharedRenderTargetPtr* renderTargets, const std::uint8_t targetCount, const SharedDepthStencilBufferPtr& dsBuffer)
		{
			assert(targetCount <= MAX_RENDER_TARGET_COUNT);
			//TODO : actually should set pipeline description based on PipelineState rather than set them here
			auto rtvHandles = g_RenderAllocator.Allocate<D3D12_CPU_DESCRIPTOR_HANDLE>(targetCount);
			auto& frameRenderTargets = mFrameResources[mFrameResourceIndex].renderTargets;
			for (std::size_t i = 0; i < targetCount;++i)
			{
				CacheResourceReference(renderTargets[i]);
				rtvHandles[i] = static_cast<const D3D12RenderTarget*>(renderTargets[i].get())->GetCPUHandle();
				//TODO : Is it correct to use the first render target's sample description to set the pipeline desc?
				if (i == 0)
				{
					mPipelineDesc.SampleDesc.Count = renderTargets[i]->GetSampleCount();
					mPipelineDesc.SampleDesc.Quality = renderTargets[i]->GetSampleQuality();
				}
				mPipelineDesc.RTVFormats[i] = D3D12TypeMapper::MapRenderFormat(renderTargets[i]->GetRenderFormat());
			}
			auto dsHandle = static_cast<const D3D12DepthStencilBuffer*>(dsBuffer.get())->GetCPUHandle();
			mCommandList->OMSetRenderTargets(targetCount, rtvHandles, FALSE, &dsHandle);
			mCurrentDSBuffer = dsBuffer;
			CacheResourceReference(dsBuffer);
			mPipelineDesc.NumRenderTargets = targetCount;
		}

		void D3D12Device::CommitGPUBuffer(const SharedGPUBufferPtr& pBuffer)
		{
			CacheResourceReference(pBuffer);
			auto bufferState = D3D12_RESOURCE_STATE_COMMON;
			ComPtr<ID3D12Resource> d3dResource;
			ComPtr<ID3D12Resource> intermediateResource;
			switch (pBuffer->GetType())
			{
			case GPUBufferType::VERTEX:
			{
				D3D12VertexBuffer* pvb = static_cast<D3D12VertexBuffer*>(pBuffer.get());
				bufferState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
				d3dResource = pvb->GetResource();
				intermediateResource = pvb->GetIntermediateResource();
				break;
			}
			case GPUBufferType::INDEX:
			{
				D3D12IndexBuffer* pib = static_cast<D3D12IndexBuffer*>(pBuffer.get());
				bufferState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
				d3dResource = pib->GetResource();
				intermediateResource = pib->GetIntermediateResource();
				break;
			}
			default:
				break;
			}
			assert(d3dResource && intermediateResource);
			mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dResource.Get(), bufferState, D3D12_RESOURCE_STATE_COPY_DEST));
			
			D3D12_SUBRESOURCE_DATA subResourceData{};
			subResourceData.pData = pBuffer->GetBuffer();
			subResourceData.RowPitch = pBuffer->GetBufferSize();
			subResourceData.SlicePitch = pBuffer->GetBufferSize();
			UpdateSubresources(mCommandList.Get(), d3dResource.Get(), intermediateResource.Get(), 0, 0, 1, &subResourceData);

			mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, bufferState));
		}

		void D3D12Device::BindGPUBuffers(std::uint8_t startSlot, const std::vector<SharedGPUBufferPtr>& pBuffers)
		{
			if (pBuffers.empty())
				return;
			auto bufferType = pBuffers[0]->GetType();
			switch (bufferType)
			{
			case GPUBufferType::VERTEX:
			{
				D3D12_VERTEX_BUFFER_VIEW* bufferViews = g_RenderAllocator.Allocate<D3D12_VERTEX_BUFFER_VIEW>(pBuffers.size());
				for (std::uint8_t i = startSlot; i < startSlot + pBuffers.size();++i)
				{
					bufferViews[i] = static_cast<D3D12VertexBuffer*>(pBuffers[i].get())->GetBufferView();
					CacheResourceReference(pBuffers[i]);
				}
				mCommandList->IASetVertexBuffers(startSlot, pBuffers.size(), bufferViews);
				break;
			}
			case GPUBufferType::INDEX:
			{
				D3D12_INDEX_BUFFER_VIEW* bufferView = g_RenderAllocator.Allocate<D3D12_INDEX_BUFFER_VIEW>(1);
				bufferView[0] = static_cast<D3D12IndexBuffer*>(pBuffers[0].get())->GetBufferView();
				CacheResourceReference(pBuffers[0]);
				mCommandList->IASetIndexBuffer(bufferView);
				break;
			}
			default:
				break;
			}
		}

		void D3D12Device::DrawVertex(const std::size_t vertexCountPerInstance, const std::size_t instanceCount, const std::size_t firstVertexIndex, const std::size_t instanceDataOffset)
		{
			//TODO : set topology based on pipeline state
			mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mCommandList->DrawInstanced(vertexCountPerInstance, instanceCount, firstVertexIndex, instanceDataOffset);
		}

		void D3D12Device::DrawIndexed(const std::size_t indexCountPerInstance, const std::size_t instanceCount, const std::size_t firstIndex, const std::size_t indexDataOffset, const std::size_t instanceDataOffset)
		{
			//TODO : set topology based on pipeline state
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
			mCommandList->RSSetScissorRects(1, &scissorRect);
			mCommandList->RSSetViewports(1, &vp);
			mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mCommandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, firstIndex, indexDataOffset, instanceDataOffset);
		}

		void D3D12Device::CacheResourceReference(const SharedDepthStencilBufferPtr& resource)
		{
			auto& frameDSBuffers = mFrameResources[mFrameResourceIndex].depthStencilBuffers;
			auto rawPointer = resource.get();
			if (frameDSBuffers.find(rawPointer) == frameDSBuffers.end())
				frameDSBuffers.emplace(rawPointer, resource);
		}

		void D3D12Device::CacheResourceReference(const SharedRenderTargetPtr& resource)
		{
			auto& frameRenderTargets = mFrameResources[mFrameResourceIndex].renderTargets;
			auto rawPointer = resource.get();
			if (frameRenderTargets.find(rawPointer) == frameRenderTargets.end())
				frameRenderTargets.emplace(rawPointer, resource);
		}

		void D3D12Device::CacheResourceReference(const SharedGPUBufferPtr& resource)
		{
			auto& frameGPUBuffers = mFrameResources[mFrameResourceIndex].buffers;
			auto rawPointer = resource.get();
			if (frameGPUBuffers.find(rawPointer) == frameGPUBuffers.end())
				frameGPUBuffers.emplace(rawPointer, resource);
		}

	}
}