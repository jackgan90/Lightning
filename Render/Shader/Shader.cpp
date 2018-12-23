#include <boost/functional/hash.hpp>
#include "Shader.h"

namespace Lightning
{
	namespace Render
	{
		size_t Shader::Hash(const ShaderType& type, const std::string& shaderName, const IShaderMacros* shaderMacros)
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, shaderName);
			boost::hash_combine(seed, shaderMacros->GetHash());
			boost::hash_combine(seed, type);
			return seed;
		}
		
		Shader::Shader(ShaderType type, const std::string& name, const char* const source):
			mType(type), mName(name), mSource(source)
		{

		}

		Shader::~Shader()
		{

		}

		void Shader::DefineMacros(const IShaderMacros* macros)
		{
			//mMacros += define;
			//SetHashDirty();
		}

		std::size_t Shader::GetHash()const
		{
			return Hash(mType, mName, &mMacros);
		}

		ShaderType Shader::GetType()const
		{
			return mType;
		}

		const char* const Shader::GetSource()const
		{
			return mSource;
		}

		const char* Shader::GetName()const
		{
			return mName.c_str();
		}

	}
}