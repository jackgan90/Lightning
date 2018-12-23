#pragma once
#include <cstddef>
#include "Portable.h"

namespace Lightning
{
	namespace Render
	{
		struct MacroPair
		{
			const char* name;
			const char* value;
		};

		struct IShaderMacros
		{
			virtual INTERFACECALL ~IShaderMacros() = default;
			virtual bool INTERFACECALL HasMacro(const char* macroName)const = 0;
			virtual void INTERFACECALL Define(const char* macroName, const char* macroValue) = 0;
			virtual void INTERFACECALL Undefine(const char* macroName) = 0;
			virtual const char* INTERFACECALL GetMacroValue(const char*  macroName)const = 0;
			virtual std::size_t INTERFACECALL GetMacroCount()const = 0;
			virtual const char* INTERFACECALL GetMacroString()const = 0;
			//Get all macros.Caller must ensure the address pointed to by *pairs has enough memory to 
			//store all of the macros.(Possibly by call GetMacroCount and pre-allocate enough memory)
			virtual void INTERFACECALL GetAllMacros(MacroPair** pairs)const = 0;
			virtual std::size_t INTERFACECALL GetHash()const = 0;
		};

	}
}