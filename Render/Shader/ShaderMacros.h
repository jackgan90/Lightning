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
			bool operator==(const ShaderMacros& define)const;
			ShaderMacros operator+(const ShaderMacros& define)const;
			ShaderMacros operator-(const ShaderMacros& define)const;
			ShaderMacros& operator+=(const ShaderMacros& define);
			ShaderMacros& operator-=(const ShaderMacros& define);
			void Combine(const ShaderMacros& define);
			void Exclude(const ShaderMacros& define);
			bool HasMacro(const std::string& macroName)const;
			void Define(const std::string& macroName, const std::string& macroValue);
			const std::string GetMacroValue(const std::string& macroName)const;
			const MacroContainer& GetAllMacros()const;
			size_t GetMacroCount()const;
		private:
			MacroContainer mMacros;
		};
	}
}
