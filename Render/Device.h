#pragma once
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
			std::shared_ptr<IShader> GetDefaultShader(ShaderType type)override;
			void CreateShaderFromFile(ShaderType type, const std::string& path, ResourceAsyncCallback<IShader> callback)override;
			void CreateTextureFromFile(const std::string& path, ResourceAsyncCallback<ITexture> callback)override;
		protected:
			Device();
			Loading::ILoader* GetLoader();
			using DefaultShaderMap = std::unordered_map<ShaderType, std::shared_ptr<IShader>>;
			DefaultShaderMap mDefaultShaders;
		private:
			Loading::ILoader* mLoader;
		};
	}
}
