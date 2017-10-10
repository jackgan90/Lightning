#pragma once
#include "filesystem.h"
#include "ishadermanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Foundation::FileSystemPtr;
		//we should not only care about the shader name but also its' defined macros since every different macro group needs to compile the shader again
		typedef std::unordered_map<std::size_t, ShaderPtr> ShaderMap;
		class ShaderManager : public IShaderManager
		{
		public:
			ShaderManager(const FileSystemPtr& fs);
			ShaderPtr GetShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap)override;
		protected:
			virtual ShaderPtr CreateConcreteShader(ShaderType type) = 0;
			ShaderMap m_shaders;
			FileSystemPtr m_fs;
		};
	}
}