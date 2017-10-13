#include "d3d12shadermanager.h"
#include "d3d12shader.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12ShaderManager::D3D12ShaderManager(const SharedFileSystemPtr& fs) :ShaderManager(fs)
		{

		}

		SharedShaderPtr D3D12ShaderManager::CreateConcreteShader(ShaderType type)
		{
			return SharedShaderPtr(new D3D12Shader(type));
		}

	}
}