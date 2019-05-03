#pragma once
#include "IShader.h"
#include "ShaderMacros.h"
#include "RefObject.h"

namespace Lightning
{
	namespace Render
	{
		class Shader : public IShader
		{
		public:
			static size_t Hash(const ShaderType& type, const std::string& shaderName, const IShaderMacros* macros);
			Shader(ShaderType type, const std::string& name, const char* const source);
			~Shader()override;
			void DefineMacros(const IShaderMacros* macros)override;
			ShaderType GetType()const override;
			const char* const GetSource()const override;
			const char* GetName()const override;
			std::size_t GetHash()const override;
		protected:
			ShaderType mType;
			std::string mName;
			const char* const mSource;
			ShaderMacros mMacros;
		};
	}
}

namespace std
{
	template<> struct hash<Lightning::Render::Shader>
	{
		std::size_t operator()(const Lightning::Render::Shader& shader)const noexcept
		{
			return Lightning::Render::Shader::Hash(shader.GetType(), shader.GetName(), shader.GetMacros());
		}
	};
}
