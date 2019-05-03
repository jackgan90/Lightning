#pragma once
#include <string>
#include <unordered_map>
#include "IShaderMacros.h"


namespace Lightning
{
	namespace Render
	{
		class ShaderMacros : public IShaderMacros
		{
		public:
			ShaderMacros();
			bool operator==(const ShaderMacros& shaderMacros)const;
			ShaderMacros operator+(const ShaderMacros& shaderMacros)const;
			ShaderMacros operator-(const ShaderMacros& shaderMacros)const;
			ShaderMacros& operator+=(const ShaderMacros& shaderMacros);
			ShaderMacros& operator-=(const ShaderMacros& shaderMacros);
			void Combine(const ShaderMacros& shaderMacros);
			void Exclude(const ShaderMacros& shaderMacros);
			bool HasMacro(const char* macroName)const override;
			void Define(const char* macroName, const char* macroValue)override;
			void Undefine(const char* macroName)override;
			const char* GetMacroValue(const char* macroName)const override;
			size_t GetMacroCount()const override;
			const char* GetMacroString()const override;
			void GetAllMacros(MacroPair** pairs)const override;
			std::size_t GetHash()const override;
		private:
			using MacroMap = std::unordered_map<std::string, std::string>;
			MacroMap mMacros;
		};
	}
}
