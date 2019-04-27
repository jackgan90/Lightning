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
			bool INTERFACECALL HasMacro(const char* macroName)const override;
			void INTERFACECALL Define(const char* macroName, const char* macroValue)override;
			void INTERFACECALL Undefine(const char* macroName)override;
			const char* INTERFACECALL GetMacroValue(const char* macroName)const override;
			size_t INTERFACECALL GetMacroCount()const override;
			const char* INTERFACECALL GetMacroString()const override;
			void INTERFACECALL GetAllMacros(MacroPair** pairs)const override;
			std::size_t INTERFACECALL GetHash()const override;
		private:
			using MacroMap = std::unordered_map<std::string, std::string>;
			MacroMap mMacros;
		};
	}
}
