#include <boost/functional/hash.hpp>
#include "Shader.h"
#include "ShaderMacros.h"

namespace Lightning
{
	namespace Render
	{
		size_t Shader::Hash(const ShaderType& type, const std::string& shaderName, const IShaderMacros* shaderMacros)
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, shaderName);
			if (shaderMacros)
			{
				boost::hash_combine(seed, shaderMacros->GetHash());
			}
			boost::hash_combine(seed, type);
			return seed;
		}
		
		Shader::Shader(ShaderType type, const std::string& name, const std::string& source, const std::shared_ptr<IShaderMacros>& macros):
			mType(type), mName(name), mSource(source), mMacros(macros)
		{

		}

		Shader::~Shader()
		{

		}

		void Shader::DefineMacro(const std::string& macroName, const std::string& macroValue)
		{
			if (!mMacros)
			{
				mMacros = std::make_shared<ShaderMacros>();
			}
			mMacros->Define(macroName, macroValue);
		}

		std::shared_ptr<IShaderMacros> Shader::GetMacros()const
		{
			return mMacros;
		}

		std::size_t Shader::GetHash()const
		{
			return Hash(mType, mName, mMacros.get());
		}

		ShaderType Shader::GetType()const
		{
			return mType;
		}

		std::string Shader::GetSource()const
		{
			return mSource;
		}

		std::string Shader::GetName()const
		{
			return mName;
		}

	}
}