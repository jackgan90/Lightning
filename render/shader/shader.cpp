#include <boost/functional/hash.hpp>
#include "EngineAlgo.h"
#include "ishader.h"

namespace Lightning
{
	namespace Render
	{
		size_t Shader::Hash(const ShaderType& type, const std::string& shaderName, const ShaderDefine& defineMap)
		{
			std::size_t hash = Utility::CalculateHash(shaderName, defineMap.GetAllMacros());
			boost::hash_combine(hash, type);
			return hash;
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