#pragma once
#include <cstddef>
#include "Portable.h"

namespace Lightning
{
	namespace Render
	{
		struct IShaderMacros
		{
			virtual INTERFACECALL ~IShaderMacros() = default;
			virtual void INTERFACECALL Combine(const IShaderMacros* shaderMacros) = 0;
			virtual void INTERFACECALL Exclude(const IShaderMacros* shaderMacros) = 0;
			virtual bool INTERFACECALL HasMacro(const char* macroName)const = 0;
			virtual void INTERFACECALL Define(const char* macroName, const char* macroValue) = 0;
			virtual const char* INTERFACECALL GetMacroValue(const char* macroName)const = 0;
			virtual std::size_t INTERFACECALL GetMacroCount()const = 0;
			virtual const char* INTERFACECALL GetMacroString()const = 0;
		};
	}
}