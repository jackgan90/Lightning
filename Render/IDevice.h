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
		using ShaderLoadFinishHandler = std::function<void(IShader*)>;
		using TextureLoadFinishHandler = std::function<void(ITexture*)>;
		class IDevice
		{
		public:
			virtual ~IDevice() = default;
			virtual IVertexBuffer* CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor) = 0;
			virtual IIndexBuffer* CreateIndexBuffer(std::uint32_t bufferSize, IndexType type) = 0;
			virtual IShader* CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap) = 0;
			virtual void CreateShaderFromFile(ShaderType type, const std::string& path,
				const ShaderDefine& defineMap, ShaderLoadFinishHandler handler) = 0;
			virtual ITexture* CreateTexture(const TextureDescriptor& descriptor, ISerializeBuffer* buffer) = 0;
			virtual void CreateTextureFromFile(const std::string& path, TextureLoadFinishHandler handler) = 0;
			virtual IShader* GetDefaultShader(ShaderType type) = 0;
		};
	}
}
