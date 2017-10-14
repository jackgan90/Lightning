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
			SharedShaderPtr GetShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap)override;
			IMemoryAllocator* GetCompileAllocator()const override{ return m_compileAllocator.get();}
		protected:
			virtual SharedShaderPtr CreateConcreteShader(ShaderType type) = 0;
			ShaderMap m_shaders;
			SharedFileSystemPtr m_fs;
			std::unique_ptr<IMemoryAllocator> m_compileAllocator;
		};
	}
}