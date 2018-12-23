#include <boost/functional/hash.hpp>
#include "IShader.h"

namespace Lightning
{
	namespace Render
	{
		size_t Shader::Hash(const ShaderType& type, const std::string& shaderName, const ShaderMacros& shaderMacros)
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, shaderName);
			boost::hash_combine(seed, shaderMacros.GetHash());
			boost::hash_combine(seed, type);
			return seed;
		}
		
		Shader::Shader(ShaderType type, const std::string& name, const std::string& entryPoint, const char* const source):
			mType(type), mName(name), mSource(source)
		{

		}

		Shader::~Shader()
		{

		}

		size_t Shader::CalculateHashInternal()
		{
			return Hash(GetType(), GetName(), GetMacros());
		}

		void Shader::DefineMacros(const ShaderMacros& define)
		{
			mMacros += define;
			SetHashDirty();
		}

		ShaderType Shader::GetType()const
		{
			return mType;
		}

		const char* const Shader::GetSource()const
		{
			return mSource;
		}

		std::string Shader::GetName()const
		{
			return mName;
		}

		void Shader::GetShaderModelVersion(int& major, int& minor)
		{
			major = mShaderModelMajorVersion;
			minor = mShaderModelMinorVersion;
		}

	}
}