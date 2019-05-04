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
			bool IsDefined(const std::string& macroName)const override;
			void Define(const std::string& macroName, const std::string& macroValue)override;
			void Undefine(const std::string& macroName)override;
			bool GetMacroValue(const std::string& macroName, std::string& macroValue)const override;
			size_t GetMacroCount()const override;
			std::string GetMacroString()const override;
			void GetAllMacros(std::vector<std::pair<std::string, std::string>>& macros)const override;
			std::size_t GetHash()const override;
		private:
			std::unordered_map<std::string, std::string> mMacros;
		};
	}
}
