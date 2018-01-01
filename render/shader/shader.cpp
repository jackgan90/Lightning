#include <boost/functional/hash.hpp>
#include "enginealgo.h"
#include "ishader.h"

namespace LightningGE
{
	namespace Render
	{
		size_t Shader::Hash(const ShaderType& type, const std::string& shaderName, const ShaderDefine& defineMap)
		{
			std::size_t hash = Utility::CalculateHash(shaderName, defineMap.GetAllDefine());
			boost::hash_combine(hash, type);
			return hash;
		}

		Shader::Shader(const std::string& entryPoint):m_entryPoint(entryPoint)
		{

		}

		Shader::~Shader()
		{

		}

		size_t Shader::CalculateHashInternal()
		{
			return Hash(GetType(), GetName(), GetMacros());
		}

		void Shader::SetEntryPoint(const std::string& entryPoint)
		{
			//TODO recompile
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