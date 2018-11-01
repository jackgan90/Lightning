#pragma once
#include <memory>
#include <functional>
#include "container.h"
#include "rendererexportdef.h"
#include "irendertarget.h"
#include "idepthstencilbuffer.h"
#include "pipelinestate.h"
#include "igpubuffer.h"
#include "ivertexbuffer.h"
#include "iindexbuffer.h"
#include "ishader.h"
#include "rect.h"
#include "color.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::container;
		using ShaderLoadFinishHandler = std::function<void(const SharedShaderPtr&)>;
		class LIGHTNING_RENDER_API IDevice
		{
		public:
			virtual ~IDevice() = default;
			virtual SharedVertexBufferPtr CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor) = 0;
			virtual SharedIndexBufferPtr CreateIndexBuffer(std::uint32_t bufferSize, IndexType type) = 0;
			virtual SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap) = 0;
			virtual void CreateShaderFromFile(ShaderType type, const std::string& path,
				const ShaderDefine& defineMap, ShaderLoadFinishHandler handler) = 0;
			virtual SharedShaderPtr GetDefaultShader(ShaderType type) = 0;
		};
		using SharedDevicePtr = std::shared_ptr<IDevice>;
	}
}
