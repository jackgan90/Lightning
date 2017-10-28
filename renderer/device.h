#pragma once
#include <memory>
#include "idevice.h"
#include "ishadermanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API Device : public IDevice
		{
		public:
			SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap)override;
		protected:
			std::unique_ptr<IShaderManager> m_shaderMgr;
		};
	}
}