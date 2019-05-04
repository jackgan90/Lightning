#pragma once
#include <unordered_map>
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
			void CreateShaderFromFile(ShaderType type, const std::string& path, 
				const std::shared_ptr<IShaderMacros>& macros, ResourceAsyncCallback<IShader> callback)override;
			void CreateTextureFromFile(const std::string& path, ResourceAsyncCallback<ITexture> callback)override;
		protected:
			Device();
			Loading::ILoader* GetLoader();
			std::unordered_map<ShaderType, std::shared_ptr<IShader>> mDefaultShaders;
		private:
			Loading::ILoader* mLoader;
		};
	}
}
