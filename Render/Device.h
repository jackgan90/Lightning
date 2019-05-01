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
			IShader* INTERFACECALL GetDefaultShader(ShaderType type)override;
			void CreateShaderFromFile(ShaderType type, const std::string& path, ResourceAsyncCallback<IShader> callback)override;
			void CreateTextureFromFile(const std::string& path, ResourceAsyncCallback<ITexture> callback)override;
		protected:
			Device();
			Loading::ILoader* GetLoader();
			using DefaultShaderMap = std::unordered_map<ShaderType, IShader*>;
			DefaultShaderMap mDefaultShaders;
		private:
			Loading::ILoader* mLoader;
		};
	}
}
