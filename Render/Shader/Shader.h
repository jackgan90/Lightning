#pragma once
#include "IShader.h"
#include "RefObject.h"

namespace Lightning
{
	namespace Render
	{
		class Shader : public IShader
		{
		public:
			static size_t Hash(const ShaderType& type, const std::string& shaderName, const IShaderMacros* macros);
			Shader(ShaderType type, const std::string& name, const std::string& source, const std::shared_ptr<IShaderMacros>& macros);
			~Shader()override;
			void DefineMacro(const std::string& macroName, const std::string& macroValue)override;
			std::shared_ptr<IShaderMacros> GetMacros()const;
			ShaderType GetType()const override;
			std::string GetSource()const override;
			std::string GetName()const override;
			std::size_t GetHash()const override;
		protected:
			ShaderType mType;
			std::string mName;
			const std::string mSource;
			std::shared_ptr<IShaderMacros> mMacros;
		};
	}
}

namespace std
{
	template<> struct hash<Lightning::Render::Shader>
	{
		std::size_t operator()(const Lightning::Render::Shader& shader)const noexcept
		{
			return Lightning::Render::Shader::Hash(shader.GetType(), shader.GetName(), shader.GetMacros().get());
		}
	};
}
