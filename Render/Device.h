#pragma once
#include <memory>
#include "IDevice.h"
#include "ILoader.h"

namespace Lightning
{
	namespace Render
	{
		class Device : public IDevice
		{
		public:
			friend class Renderer;
			~Device()override;
			IShader* GetDefaultShader(ShaderType type)override;
			void CreateShaderFromFile(ShaderType type, const std::string& path,
				const ShaderMacros& macros, ShaderLoadFinishHandler handler)override;
			void CreateTextureFromFile(const std::string& path, TextureLoadFinishHandler handler)override;
		protected:
			Device();
			using DefaultShaderMap = Container::UnorderedMap<ShaderType, IShader*>;
			DefaultShaderMap mDefaultShaders;
			Loading::ILoader* mLoader;
		};
	}
}
