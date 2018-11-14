#pragma once
#include <memory>
#include "IDevice.h"

namespace Lightning
{
	namespace Render
	{
		class Device : public IDevice
		{
		public:
			friend class Renderer;
			Device();
			SharedShaderPtr GetDefaultShader(ShaderType type)override;
			void CreateShaderFromFile(ShaderType type, const std::string& path,
				const ShaderDefine& defineMap, ShaderLoadFinishHandler handler)override;
			void CreateTextureFromFile(const TextureDescriptor& descriptor, const std::string& path,
				TextureLoadFinishHandler handler)override;
		protected:
			using DefaultShaderMap = Container::UnorderedMap<ShaderType, SharedShaderPtr>;
			DefaultShaderMap mDefaultShaders;
		};
	}
}
