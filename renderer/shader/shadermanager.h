#pragma once
#include <memory>
#include "filesystem.h"
#include "ishadermanager.h"
#include "stackallocator.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Foundation::SharedFileSystemPtr;
		//we should not only care about the shader name but also its' defined macros since every different macro group needs to compile the shader again
		typedef std::unordered_map<std::size_t, SharedShaderPtr> ShaderMap;
		class ShaderManager : public IShaderManager
		{
		public:
			ShaderManager(const SharedFileSystemPtr& fs);
			SharedShaderPtr CreateShaderFromFile(ShaderType type, const std::string& shaderFileName, const ShaderDefine& defineMap)override;
			SharedShaderPtr GetShader(size_t shaderHash)override;
			IMemoryAllocator* GetCompileAllocator()const override{ return m_compileAllocator.get();}
		protected:
			virtual SharedShaderPtr CreateConcreteShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap) = 0;
			ShaderMap m_shaders;
			SharedFileSystemPtr m_fs;
			std::unique_ptr<IMemoryAllocator> m_compileAllocator;
		};
	}
}