#include <boost/functional/hash.hpp>
#include "enginealgo.h"
#include "ishader.h"

namespace LightningGE
{
	namespace Render
	{
		StackAllocator<true, 16, 8192 > Shader::s_compileAllocator;
		size_t Shader::Hash(const ShaderType& type, const std::string& shaderName, const ShaderDefine& defineMap)
		{
			std::size_t hash = Utility::CalculateHash(shaderName, defineMap.GetAllDefine());
			boost::hash_combine(hash, type);
			return hash;
		}
		
		Shader::Shader(ShaderType type, const std::string& name, const std::string& entryPoint, const char* const source):
			m_type(type), m_name(name), m_entryPoint(entryPoint), m_source(source)
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

		ShaderType Shader::GetType()const
		{
			return m_type;
		}

		const char* const Shader::GetSource()const
		{
			return m_source;
		}

		std::string Shader::GetName()const
		{
			return m_name;
		}

		void Shader::GetShaderModelVersion(int& major, int& minor)
		{
			major = m_smMajorVersion;
			minor = m_smMinorVersion;
		}

	}
}