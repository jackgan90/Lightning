#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "container.h"
#include "d3d12shadermanager.h"
#include "d3d12descriptorheapmanager.h"
#include "ishader.h"

namespace Lightning
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		using Foundation::container;
		enum class D3D12RootBoundResourceType
		{
			DescriptorTable,
			ConstantBufferView,
			ShaderResourceView,
			UnorderedAccessView,
			Constant
		};

		struct D3D12Constant32BitValue
		{
			const void* p32BitValues;
			UINT num32BitValues;
			UINT dest32BitValueOffset;
		};

		struct D3D12RootBoundResource
		{
			D3D12RootBoundResourceType type;
			ComPtr<ID3D12DescriptorHeap> descriptorTableHeap;
			union
			{
				D3D12_GPU_DESCRIPTOR_HANDLE descriptorTableHandle;
				D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress;
				D3D12Constant32BitValue constant32BitValue;
			};
		};

		class D3D12Shader : public Shader
		{
		public:
			friend class D3D12Device;
			friend class D3D12ShaderManager;
			D3D12Shader(ID3D12Device* device, ShaderType type, const std::string& name, const std::string& entry, const char* const shaderSource);
			~D3D12Shader()override;
			const ShaderDefine GetMacros()const override;
			//bool Compile(const Foundation::SharedFilePtr& file, const ShaderDefine& define)override;
			//const std::string GetCompileErrorLog()const override;
			std::size_t GetArgumentCount()const override;
			void SetArgument(const ShaderArgument& argument) override;
			void Compile()override;
			void* GetByteCodeBuffer()const;
			SIZE_T GetByteCodeBufferSize()const;
			const container::vector<D3D12_ROOT_PARAMETER>& GetRootParameters()const;
			std::size_t GetRootParameterCount()const;
			const container::vector<D3D12RootBoundResource>& GetRootBoundResources();
		private:
			struct ArgumentBinding
			{
				UINT bufferIndex;
				UINT offsetInBuffer;
			};
			void CompileImpl();
			D3D12_SHADER_VISIBILITY GetParameterVisibility()const;
			void UpdateRootBoundResources();
			ComPtr<ID3D10Blob> mByteCode;
			D3D12_SHADER_DESC mDesc;
			DescriptorHeap *mConstantHeaps[RENDER_FRAME_COUNT];
			container::unordered_map<std::string, ArgumentBinding> mArgumentBindings;
			container::vector<D3D12_ROOT_PARAMETER> mRootParameters;
			container::unordered_map<std::string, D3D12_SHADER_INPUT_BIND_DESC> mInputBindDescs;
			container::unordered_map<std::size_t, D3D12_SHADER_BUFFER_DESC> mBufferDescs;
			container::unordered_map<std::uint8_t, container::vector<D3D12RootBoundResource>> mRootBoundResources;
			//bufferIndex to bufferid
			container::unordered_map<UINT, std::size_t> mBoundBufferCache;
			D3D12_DESCRIPTOR_RANGE *mDescriptorRanges;
		};
	}
}