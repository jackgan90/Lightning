#pragma once
#include "shadermanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		class D3D12Device;
		class D3D12ShaderManager : public ShaderManager
		{
		public:
			D3D12ShaderManager(D3D12Device* pDevice, const SharedFileSystemPtr& fs);
		protected:
			SharedShaderPtr CreateConcreteShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap)override;
		private:
			D3D12Device* m_device;
		};
	}
}