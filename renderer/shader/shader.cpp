#include <boost/functional/hash.hpp>
#include "enginealgo.h"
#include "ishader.h"

namespace LightningGE
{
	namespace Renderer
	{
		size_t Shader::Hash(const ShaderType& type, const std::string& shaderName, const ShaderDefine& defineMap)
		{
			std::size_t hash = Utility::CalculateHash(shaderName, defineMap.GetAllDefine());
			boost::hash_combine(hash, type);
			return hash;
		}

		Shader::Shader():m_entryPoint("main")
		{

		}


		size_t Shader::CalculateHashInternal()
		{
			return Hash(GetType(), GetName(), GetMacros());
		}

		void Shader::SetEntryPoint(const std::string& entryPoint)
		{
			bool changed = m_entryPoint != entryPoint;
			m_entryPoint = entryPoint;
			if (changed)
				SetHashDirty();
		}

		void Shader::DefineMacros(const ShaderDefine& define)
		{
			m_macros += define;
			SetHashDirty();
		}
	}
}