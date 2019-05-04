#pragma once
#include <cstddef>
#include "Portable.h"

namespace Lightning
{
	namespace Render
	{
		struct IShaderMacros
		{
			virtual ~IShaderMacros() = default;
			virtual bool IsDefined(const std::string& macroName)const = 0;
			virtual void Define(const std::string& macroName, const std::string& macroValue) = 0;
			virtual void Undefine(const std::string& macroName) = 0;
			virtual bool GetMacroValue(const std::string& macroName, std::string& macroValue)const = 0;
			virtual std::size_t GetMacroCount()const = 0;
			virtual std::string GetMacroString()const = 0;
			virtual void GetAllMacros(std::vector<std::pair<std::string, std::string>>& macros)const = 0;
			virtual std::size_t GetHash()const = 0;
		};

	}
}