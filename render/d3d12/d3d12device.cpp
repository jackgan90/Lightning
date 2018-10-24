#include <d3dx12.h>
#include <d3dcompiler.h>
#include <functional>
#include "common.h"
#include "irenderer.h"
#include "d3d12device.h"
#include "d3d12rendertarget.h"
#include "d3d12depthstencilbuffer.h"
#include "d3d12typemapper.h"
#include "d3d12vertexbuffer.h"
#include "d3d12indexbuffer.h"
#include "renderconstants.h"
#include "semantics.h"
#include "shadermanager.h"
#include "logger.h"
#include "configmanager.h"
#include "framememoryallocator.h"
#include "d3d12descriptorheapmanager.h"
#include "d3d12constantbuffermanager.h"
#include "tbb/spin_mutex.h"

namespace
{
	static tbb::spin_mutex mtxPipelineCache;
	static tbb::spin_mutex mtxRootSignature;
}

namespace Lightning
{
	namespace Render
	{
		using Foundation::ConfigManager;
		using Foundation::EngineConfig;
		using Foundation::FrameMemoryAllocator;

		extern FrameMemoryAllocator g_RenderAllocator;
		const char* const DEFAULT_VS_SOURCE =
			"cbuffer VSConstants : register(b0)\n"
			"{\n"
			"	float4x4 wvp;\n"
			"};\n"
			"struct VSInput\n"
			"{\n"
			"	float3 position : POSITION;\n"
			"	float3 normal : NORMAL;\n"
			"};\n"
			"struct VSOutput\n"
			"{\n"
			"	float4 pos : SV_POSITION;\n"
			"	float3 normal : TEXCOORD0;\n"
			"};\n"
			"VSOutput main(VSInput input)\n"
			"{\n"
			"	VSOutput output;\n"
			"	output.pos = mul(wvp, float4(input.position, 1.0f));\n"
			"	output.normal = input.normal;\n"
			"	return output;\n"
			"}\n";
		const char* const DEFAULT_PS_SOURCE =
			"cbuffer PSConstants : register(b0)\n"
			"{\n"
			"	float4 color;\n"
			"	float3 light;\n"
			"};\n"
			"//cbuffer PSConstants1 : register(b1)\n"
			"//{\n"
			"//	float3 light;\n"
			"//};\n"
			"struct PSInput\n"
			"{\n"
			"	float4 pos : SV_POSITION;\n"
			"	float3 normal : TEXCOORD0;\n"
			"};\n"
			"float4 main(PSInput input):SV_TARGET\n"
			"{\n"
			"	float3 N = normalize(input.normal);\n"
			"	float3 L = normalize(light);\n"
			"	float3 diffuse = dot(N, L);\n"
			"	return float4(color.rgb * diffuse, color.a) ;\n"
			"}\n";
		D3D12Device::D3D12Device(IDXGIFactory4* factory, const SharedFileSystemPtr& fs)
			:Device()
		{
			CreateNativeDevice(factory);
			D3D12_COMMAND_QUEUE_DESC desc = {};
			HRESULT hr = mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&mCommandQueue));
			if (FAILED(hr))
			{
				throw DeviceInitException("Failed to create command queue!");
			}
			mShaderMgr = std::make_unique<D3D12ShaderManager>(this, fs);
			//should create first default pipeline state
			mDefaultShaders[ShaderType::VERTEX] = mShaderMgr->CreateShaderFromSource(ShaderType::VERTEX, "[Built-in]default.vs", DEFAULT_VS_SOURCE, ShaderDefine());
			mDefaultShaders[ShaderType::FRAGMENT] = mShaderMgr->CreateShaderFromSource(ShaderType::FRAGMENT, "[Built-in]default.ps", DEFAULT_PS_SOURCE, ShaderDefine()); 
		}

		D3D12Device::~D3D12Device()
		{
			mShaderMgr.reset();
		}

		ID3D12GraphicsCommandList* D3D12Device::GetGraphicsCommandList()
		{
			return mFrameResources[mFrameResourceIndex]->GetCommandList();
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
			auto commandList = GetGraphicsCommandList();
			if (rt->IsSwapChainRenderTarget())
			{
				commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(nativeRenderTarget.Get(),
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
			}

			//cache render target to prevent it from being released before GPU execute ClearRenderTargetView

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



		SharedVertexBufferPtr D3D12Device::CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor)
		{
			return std::make_shared<D3D12VertexBuffer>(this, bufferSize, descriptor);
		}

		SharedIndexBufferPtr D3D12Device::CreateIndexBuffer(std::uint32_t bufferSize, IndexType type)
		{
			return std::make_shared<D3D12IndexBuffer>(this, bufferSize, type);
		}

		void D3D12Device::ApplyRasterizerState(const RasterizerState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		{
			D3D12_RASTERIZER_DESC* pDesc = &desc.RasterizerState;
			pDesc->FillMode = D3D12TypeMapper::MapFillMode(state.fillMode);
			pDesc->CullMode = D3D12TypeMapper::MapCullMode(state.cullMode);
			pDesc->ForcedSampleCount = 0;
			pDesc->FrontCounterClockwise = state.frontFace == WindingOrder::COUNTER_CLOCKWISE;
		}

		void D3D12Device::ApplyBlendStates(const std::uint8_t firstRTIndex, const BlendState* states, const std::uint8_t stateCount, 
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

		void D3D12Device::ApplyDepthStencilState(const DepthStencilState& state, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
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

		void D3D12Device::ApplyPipelineState(const PipelineState& state)
		{
			PipelineStateRootSignature stateAndSignature;
			auto hashValue = std::hash<PipelineState>{}(state);
			bool createNewPSO{ true };
			{
				tbb::spin_mutex::scoped_lock lock(mtxPipelineCache);
				auto it = mPipelineCache.find(hashValue);
				if (it != mPipelineCache.end())
				{
					stateAndSignature = it->second;
					createNewPSO = false;
				}
			}

			if(createNewPSO)
			{
				stateAndSignature = CreateAndCachePipelineState(state, hashValue);
			}

			auto commandList = GetGraphicsCommandList();
			commandList->SetPipelineState(stateAndSignature.pipelineState.Get());
			if (stateAndSignature.rootSignature)
			{
				commandList->SetGraphicsRootSignature(stateAndSignature.rootSignature.Get());
				BindShaderResources(state);
			}
		}

		std::size_t D3D12Device::AnalyzeShaderRootResources(IShader *pShader, 
			container::unordered_map<ShaderType, container::vector<D3D12Device::ShaderResourceHandle>>& resourceHandles)
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

		void D3D12Device::BindShaderResources(const PipelineState& state)
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
								mDevice->CreateConstantBufferView(&cbvDesc, cpuHandle);
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

		void D3D12Device::ApplyViewports(const RectFList& vp, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		{

		}

		void D3D12Device::ApplyScissorRects(const RectFList& scissorRects, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
		{

		}

		D3D12Device::PipelineStateRootSignature D3D12Device::CreateAndCachePipelineState(const PipelineState& state, std::size_t hashValue)
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
			auto hr = mDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&stateAndSignature.pipelineState));
			if (FAILED(hr))
			{
				LOG_ERROR("Failed to apply pipeline state!");
				return stateAndSignature;
			} 
			{
				tbb::spin_mutex::scoped_lock lock(mtxPipelineCache);
				//Must check again because other threads may create the same pipeline state object simultaneously
				auto it = mPipelineCache.find(hashValue);
				if (it == mPipelineCache.end())
					mPipelineCache.emplace(hashValue, stateAndSignature);
				else
					stateAndSignature = it->second;
			}
			return stateAndSignature;
		}

		SharedShaderPtr D3D12Device::CreateShader(ShaderType type, const std::string& shaderName, 
			const char* const shaderSource, const ShaderDefine& defineMap)
		{
			return std::make_shared<D3D12Shader>(mDevice.Get(), type, shaderName, DEFAULT_SHADER_ENTRY, shaderSource);
		}

		void D3D12Device::ApplyShader(IShader* pShader, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
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

		void D3D12Device::UpdatePSOInputLayout(const VertexInputLayout *inputLayouts, std::uint8_t  layoutCount, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
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

		ComPtr<ID3D12RootSignature> D3D12Device::GetRootSignature(const container::vector<IShader*>& shaders)
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
				tbb::spin_mutex::scoped_lock lock(mtxRootSignature);
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
			hr = mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
			if (FAILED(hr))
			{
				return ComPtr<ID3D12RootSignature>();
			}

			{
				tbb::spin_mutex::scoped_lock lock(mtxRootSignature);
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

		void D3D12Device::GetAllCommandLists(std::size_t frameResourceIndex, container::vector<ID3D12CommandList*>& lists)
		{
			auto& frameResources = mFrameResources[frameResourceIndex];
			for (auto it = frameResources.begin(); it != frameResources.end(); ++it)
			{
				lists.push_back(it->GetCommandList());
			}
		}

		void D3D12Device::EndFrame(const std::size_t frameResourceIndex)
		{
		}

		void D3D12Device::BeginFrame(const std::size_t frameResourceIndex)
		{
			Device::BeginFrame(frameResourceIndex);
			D3D12DescriptorHeapManager::Instance()->EraseTransientAllocation(frameResourceIndex);
			D3D12ConstantBufferManager::Instance()->ResetBuffers(frameResourceIndex);
			auto& frameResources = mFrameResources[frameResourceIndex];
			for (auto it = frameResources.begin(); it != frameResources.end(); ++it)
			{
				it->Reset(true);
			}
		}

		void D3D12Device::ApplyRenderTargets(const container::vector<SharedRenderTargetPtr>& renderTargets, const SharedDepthStencilBufferPtr& dsBuffer)
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

		void D3D12Device::BindGPUBuffer(std::uint8_t slot, const SharedGPUBufferPtr& pBuffer)
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

		void D3D12Device::DrawVertex(const std::size_t vertexCountPerInstance, const std::size_t instanceCount, const std::size_t firstVertexIndex, const std::size_t instanceDataOffset)
		{
			auto commandList = GetGraphicsCommandList();
			//TODO : set topology based on pipeline state
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			commandList->DrawInstanced(vertexCountPerInstance, instanceCount, firstVertexIndex, instanceDataOffset);
		}

		void D3D12Device::DrawIndexed(const std::size_t indexCountPerInstance, const std::size_t instanceCount, const std::size_t firstIndex, const std::size_t indexDataOffset, const std::size_t instanceDataOffset)
		{
			auto commandList = GetGraphicsCommandList();
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
			commandList->RSSetScissorRects(1, &scissorRect);
			commandList->RSSetViewports(1, &vp);
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			commandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, firstIndex, indexDataOffset, instanceDataOffset);
		}
		//native device method wrappers begin
		ComPtr<ID3D12Resource> D3D12Device::CreateCommittedResource(
			const D3D12_HEAP_PROPERTIES *pHeapProperties,
			D3D12_HEAP_FLAGS HeapFlags,
			const D3D12_RESOURCE_DESC *pDesc,
			D3D12_RESOURCE_STATES InitialResourceState,
			const D3D12_CLEAR_VALUE *pOptimizedClearValue)
		{
			ComPtr<ID3D12Resource> resource;
			mDevice->CreateCommittedResource(pHeapProperties, HeapFlags, pDesc,
				InitialResourceState, pOptimizedClearValue, IID_PPV_ARGS(&resource));

			return resource;
		}

		void D3D12Device::CreateRenderTargetView(
			ID3D12Resource *pResource,
			const D3D12_RENDER_TARGET_VIEW_DESC *pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
		{
			mDevice->CreateRenderTargetView(pResource, pDesc, DestDescriptor);
		}

		void D3D12Device::CreateDepthStencilView(
			ID3D12Resource *pResource,
			const D3D12_DEPTH_STENCIL_VIEW_DESC *pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
		{
			mDevice->CreateDepthStencilView(pResource, pDesc, DestDescriptor);
		}

		UINT D3D12Device::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType)
		{
			return mDevice->GetDescriptorHandleIncrementSize(DescriptorHeapType);
		}

		ComPtr<ID3D12DescriptorHeap> D3D12Device::CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC *pDescriptorHeapDesc)
		{
			ComPtr<ID3D12DescriptorHeap> heap;
			mDevice->CreateDescriptorHeap(pDescriptorHeapDesc, IID_PPV_ARGS(&heap));
			return heap;
		}

		ComPtr<ID3D12CommandAllocator> D3D12Device::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type)
		{
			ComPtr<ID3D12CommandAllocator> commandAllocator;
			mDevice->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));
			return commandAllocator;
		}

		ComPtr<ID3D12CommandList> D3D12Device::CreateCommandList(
			UINT nodeMask,
			D3D12_COMMAND_LIST_TYPE type,
			ID3D12CommandAllocator *pCommandAllocator,
			ID3D12PipelineState *pInitialState)
		{
			ComPtr<ID3D12CommandList> commandList;
			mDevice->CreateCommandList(nodeMask, type, pCommandAllocator, pInitialState, IID_PPV_ARGS(&commandList));
			return commandList;
		}

		HRESULT D3D12Device::CheckFeatureSupport(D3D12_FEATURE Feature, void *pFeatureSupportData, UINT FeatureSupportDataSize)
		{
			return mDevice->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
		}

		ComPtr<ID3D12Fence> D3D12Device::CreateFence(UINT64 InitialValue, D3D12_FENCE_FLAGS Flags)
		{
			ComPtr<ID3D12Fence> fence;
			mDevice->CreateFence(InitialValue, Flags, IID_PPV_ARGS(&fence));
			return fence;
		}

		//native device method wrappers end
	}
}