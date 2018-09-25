#pragma once
#include <memory>
#include "container.h"
#include "filesystem.h"
#include "ishadermanager.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::SharedFileSystemPtr;
		using Foundation::container;
		//we should not only care about the shader name but also its' defined macros since every different macro group needs to compile the shader again
		using ShaderMap = container::unordered_map<std::size_t, SharedShaderPtr>;
		class ShaderManager : public IShaderManager
		{
		public:
			ShaderManager(const SharedFileSystemPtr& fs);
			~ShaderManager()override;
			SharedShaderPtr CreateShaderFromSource(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap)override;
			SharedShaderPtr CreateShaderFromFile(ShaderType type, const std::string& shaderFileName, const ShaderDefine& defineMap)override;
			SharedShaderPtr GetShader(size_t shaderHash)override;
		protected:
			virtual SharedShaderPtr CreateConcreteShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap) = 0;
			ShaderMap mShaders;
			SharedFileSystemPtr mFs;
		};
	}
}