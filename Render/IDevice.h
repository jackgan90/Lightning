#pragma once
#include <memory>
#include <functional>
#include "Container.h"
#include "IRenderTarget.h"
#include "IDepthStencilBuffer.h"
#include "PipelineState.h"
#include "IGPUBuffer.h"
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
#include "IShader.h"
#include "Texture/Itexture.h"
#include "Texture/Sampler.h"
#include "ISerializeBuffer.h"
#include "Rect.h"
#include "Color.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
		using Loading::ISerializeBuffer;
		//Avoid using template in cross module interfaces
		class IShaderLoadCallback
		{
		public:
			virtual ~IShaderLoadCallback() = default;
			virtual void operator()(IShader*) = 0;
		};

		class ITextureLoadCallback
		{
		public:
			virtual ~ITextureLoadCallback() = default;
			virtual void operator()(ITexture*) = 0;
		};

		class IDevice
		{
		public:
			virtual ~IDevice() = default;
			virtual IVertexBuffer* CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor) = 0;
			virtual IIndexBuffer* CreateIndexBuffer(std::uint32_t bufferSize, IndexType type) = 0;
			virtual IShader* CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderMacros& macros) = 0;
			virtual void CreateShaderFromFile(ShaderType type, const std::string& path, IShaderLoadCallback* callback) = 0;
			virtual ITexture* CreateTexture(const TextureDescriptor& descriptor, ISerializeBuffer* buffer) = 0;
			virtual void CreateTextureFromFile(const std::string& path, ITextureLoadCallback* callback) = 0;
			virtual IShader* GetDefaultShader(ShaderType type) = 0;
		};
	}
}
