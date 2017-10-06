#pragma once
#include "shadermanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		class D3D12ShaderManager : public ShaderManager
		{
		protected:
			ShaderPtr CreateConcreteShader(ShaderType type)override;
		};
	}
}