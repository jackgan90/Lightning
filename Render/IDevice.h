#pragma once
#include <functional>
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
#include "IShader.h"
#include "Texture/Itexture.h"
#include "ISerializeBuffer.h"

namespace Lightning
{
	namespace Render
	{
		using Loading::ISerializeBuffer;
		//Avoid using template in cross module interfaces
		struct IShaderCallback
		{
			virtual ~IShaderCallback() = default;
			virtual void Execute(IShader*) = 0;
		};

		struct ITextureCallback
		{
			virtual ~ITextureCallback() = default;
			virtual void Execute(ITexture*) = 0;
		};

		struct IDevice
		{
			virtual INTERFACECALL ~IDevice() = default;
			virtual IVertexBuffer* INTERFACECALL CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor) = 0;
			virtual IIndexBuffer* INTERFACECALL CreateIndexBuffer(std::uint32_t bufferSize, IndexType type) = 0;
			virtual IShader* INTERFACECALL CreateShader(ShaderType type, const char* shaderName, 
				const char* const shaderSource, const IShaderMacros* macros) = 0;
			virtual void INTERFACECALL CreateShaderFromFile(ShaderType type, const char* const path, IShaderCallback* callback) = 0;
			virtual ITexture* INTERFACECALL CreateTexture(const TextureDescriptor& descriptor, ISerializeBuffer* buffer) = 0;
			virtual void INTERFACECALL CreateTextureFromFile(const char* const path, ITextureCallback* callback) = 0;
			virtual IShader* INTERFACECALL GetDefaultShader(ShaderType type) = 0;
		};
	}
}
