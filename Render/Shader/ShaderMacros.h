#pragma once
#include <memory>
#include <string>
#include "Container.h"


namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
		using MacroContainer = Container::UnorderedMap<std::string, std::string>;
		class ShaderMacros
		{
		public:
			ShaderMacros();
			ShaderMacros(const MacroContainer& map);
			bool operator==(const ShaderMacros& shaderMacros)const;
			ShaderMacros operator+(const ShaderMacros& shaderMacros)const;
			ShaderMacros operator-(const ShaderMacros& shaderMacros)const;
			ShaderMacros& operator+=(const ShaderMacros& shaderMacros);
			ShaderMacros& operator-=(const ShaderMacros& shaderMacros);
			void Combine(const ShaderMacros& shaderMacros);
			void Exclude(const ShaderMacros& shaderMacros);
			bool HasMacro(const std::string& macroName)const;
			void Define(const std::string& macroName, const std::string& macroValue);
			const std::string GetMacroValue(const std::string& macroName)const;
			const MacroContainer& GetAllMacros()const;
			size_t GetMacroCount()const;
			std::string GetMacroString()const;
		private:
			MacroContainer mMacros;
		};
	}
}
