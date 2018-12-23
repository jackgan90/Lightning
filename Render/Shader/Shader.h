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
			INTERFACECALL ~Shader()override;
			void INTERFACECALL DefineMacros(const IShaderMacros* macros)override;
			ShaderType INTERFACECALL GetType()const override;
			const char* const INTERFACECALL GetSource()const override;
			const char* INTERFACECALL GetName()const override;
			std::size_t INTERFACECALL GetHash()const override;
		protected:
			ShaderType mType;
			std::string mName;
			const char* const mSource;
			ShaderMacros mMacros;
			REF_OBJECT_OVERRIDE(Shader)
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
