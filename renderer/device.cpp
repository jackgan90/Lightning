#include <cassert>
#include "device.h"

namespace LightningGE
{
	namespace Renderer
	{
		SharedShaderPtr Device::CreateShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap)
		{
			assert(m_shaderMgr != nullptr && "You must create IShaderManager instance in derived class!");
			return m_shaderMgr->GetShader(type, shaderName, defineMap);
		}
	}
}