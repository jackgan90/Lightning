#pragma once
#include "IDevice.h"
#include "ILoader.h"
#include "Container.h"

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
			void INTERFACECALL CreateShaderFromFile(ShaderType type, const char* const path, IShaderCallback* callback)override;
			void INTERFACECALL CreateTextureFromFile(const char* const path, ITextureCallback* callback)override;
		protected:
			Device();
			Loading::ILoader* GetLoader();
			using DefaultShaderMap = Foundation::Container::UnorderedMap<ShaderType, IShader*>;
			DefaultShaderMap mDefaultShaders;
		private:
			Loading::ILoader* mLoader;
		};
	}
}
