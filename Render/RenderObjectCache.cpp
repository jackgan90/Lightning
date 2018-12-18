#include "RenderObjectCache.h"
#undef GetObject

namespace Lightning
{
	namespace Render
	{
		IShader* ShaderCache::GetShader(ShaderType type, const std::string& name, const ShaderMacros& macros)
		{
			return GetObject(GetKey(type, name, macros));
		}

		bool ShaderCache::AddShader(IShader* shader)
		{
			return AddObject(GetKey(shader->GetType(), shader->GetName(), shader->GetMacros()), shader);
		}

		std::string ShaderCache::GetKey(ShaderType type, const std::string& name, const ShaderMacros& macros)
		{
			return std::to_string(static_cast<int>(type)) + "_" + name + + "_" + macros.GetMacroString();
		}
	}
}