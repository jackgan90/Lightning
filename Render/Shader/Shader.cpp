#include <boost/functional/hash.hpp>
#include "IShader.h"

namespace Lightning
{
	namespace Render
	{
		size_t Shader::Hash(const ShaderType& type, const std::string& shaderName, const ShaderDefine& defineMap)
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, shaderName);
			auto& macros = defineMap.GetAllMacros();
			for (auto it = macros.begin(); it != macros.end(); ++it)
			{
				boost::hash_combine(seed, it->first);
				boost::hash_combine(seed, it->second);
			}
			boost::hash_combine(seed, type);
			return seed;
		}
		
		Shader::Shader(ShaderType type, const std::string& name, const std::string& entryPoint, const char* const source):
			mType(type), mName(name), mEntryPoint(entryPoint), mSource(source)
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
			bool changed = mEntryPoint != entryPoint;
			mEntryPoint = entryPoint;
			if (changed)
				SetHashDirty();
		}

		void Shader::DefineMacros(const ShaderDefine& define)
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