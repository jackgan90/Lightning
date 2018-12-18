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
			void CreateShaderFromFile(ShaderType type, const std::string& path, IShaderLoadCallback* callback)override;
			void CreateTextureFromFile(const std::string& path, ITextureLoadCallback* callback)override;
		protected:
			Device();
			Loading::ILoader* GetLoader();
			using DefaultShaderMap = Container::UnorderedMap<ShaderType, IShader*>;
			DefaultShaderMap mDefaultShaders;
		private:
			Loading::ILoader* mLoader;
		};
	}
}
