#include "RenderObjectCache.h"
#undef GetObject

namespace Lightning
{
	namespace Render
	{
		std::shared_ptr<IShader> ShaderCache::GetShader(ShaderType type, const std::string& name, const IShaderMacros* macros)
		{
			return GetObject(GetKey(type, name, macros));
		}

		bool ShaderCache::AddShader(const std::shared_ptr<IShader>& shader)
		{
			return AddObject(GetKey(shader->GetType(), shader->GetName(), shader->GetMacros()), shader);
		}

		std::string ShaderCache::GetKey(ShaderType type, const std::string& name, const IShaderMacros* macros)
		{
			return std::to_string(static_cast<int>(type)) + "_" + name + + "_" + macros->GetMacroString();
		}
	}
}