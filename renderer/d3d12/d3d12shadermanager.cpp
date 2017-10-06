#include "d3d12shadermanager.h"
#include "d3d12shader.h"

namespace LightningGE
{
	namespace Renderer
	{
		ShaderPtr D3D12ShaderManager::CreateConcreteShader(ShaderType type)
		{
			return ShaderPtr(new D3D12Shader(type));
		}

	}
}