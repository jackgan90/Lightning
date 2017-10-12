#pragma once
#include "shadermanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		class D3D12ShaderManager : public ShaderManager
		{
		public:
			D3D12ShaderManager(const SharedFileSystemPtr& fs);
		protected:
			ShaderPtr CreateConcreteShader(ShaderType type)override;
		};
	}
}