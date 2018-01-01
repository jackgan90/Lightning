#include "d3d12device.h"
#include "d3d12shadermanager.h"
#include "d3d12shader.h"

namespace LightningGE
{
	namespace Render
	{
		D3D12ShaderManager::D3D12ShaderManager(D3D12Device* pDevice, const SharedFileSystemPtr& fs) :ShaderManager(fs)
			,m_device(pDevice)
		{

		}

		SharedShaderPtr D3D12ShaderManager::CreateConcreteShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap)
		{
			return m_device->CreateShader(type, shaderName, shaderSource, defineMap);
		}

	}
}