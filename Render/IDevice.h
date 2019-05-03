#pragma once
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
#include "IShader.h"
#include "Texture/ITexture.h"
#include "ISerializeBuffer.h"
#include "IRenderTarget.h"
#include "IDepthStencilBuffer.h"

namespace Lightning
{
	namespace Render
	{
		using Loading::ISerializeBuffer;
		template<typename T>
		using ResourceAsyncCallback = std::function<void(const std::shared_ptr<T>&)>;
		
		struct IDevice
		{
			virtual ~IDevice() = default;
			//Note : Users must make sure the address pointed by descriptor.components is valid when call this method
			//After CreateVertexBuffer returns,the users are free to deallocate memory.But keep in mind:don't free the
			//memory during the inovcation.
			virtual std::shared_ptr<IVertexBuffer> CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor) = 0;
			virtual std::shared_ptr<IIndexBuffer> CreateIndexBuffer(std::uint32_t bufferSize, IndexType type) = 0;
			virtual std::shared_ptr<IShader> CreateShader(ShaderType type, const char* shaderName, 
				const char* const shaderSource, const IShaderMacros* macros) = 0;
			virtual std::shared_ptr<IRenderTarget> CreateRenderTarget(const std::shared_ptr<ITexture>& texture) = 0;
			virtual std::shared_ptr<IDepthStencilBuffer> CreateDepthStencilBuffer(const std::shared_ptr<ITexture>& texture) = 0;
			virtual void CreateShaderFromFile(ShaderType type, const std::string& path, ResourceAsyncCallback<IShader> callback) = 0;
			virtual std::shared_ptr<ITexture> CreateTexture(const TextureDescriptor& descriptor, const std::shared_ptr<ISerializeBuffer>& buffer) = 0;
			virtual void CreateTextureFromFile(const std::string& path, ResourceAsyncCallback<ITexture> callback) = 0;
			virtual std::shared_ptr<IShader> GetDefaultShader(ShaderType type) = 0;
		};
	}
}
